#include "siftmatch.h"

#define DISPLAY 1

//SIFT算法头文件
//加extern "C"，告诉编译器按C语言的方式编译和连接
extern "C"
{
#include "imgfeatures.h"
#include "kdtree.h"
#include "minpq.h"
#include "sift.h"
#include "utils.h"
#include "xform.h"
}


//在k-d树上进行BBF搜索的最大次数
/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

//目标点与最近邻和次近邻的距离的比值的阈值，若大于此阈值，则剔除此匹配点对
//通常此值取0.6，值越小找到的匹配点对越精确，但匹配数目越少
/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

//窗口名字符串
#define IMG1 "图1"
#define IMG2 "图2"
#define IMG1_FEAT "图1特征点"
#define IMG2_FEAT "图2特征点"
#define IMG_MATCH1 "距离比值筛选后的匹配结果"
#define IMG_MATCH2 "RANSAC筛选后的匹配结果"
#define IMG_MOSAIC_TEMP "临时拼接图像"
#define IMG_MOSAIC_SIMPLE "简易拼接图"
#define IMG_MOSAIC_BEFORE_FUSION "重叠区域融合前"
#define IMG_MOSAIC_PROC "处理后的拼接图"


SiftMatch::SiftMatch(const string &name1, const string &name2)
    :name1(name1), name2(name2)
{
    open_image_number = 0;//打开图片的个数

    //设定横竖排的单选按钮的勾选状态，默认是横排

    img1_Feat = NULL;
    img2_Feat = NULL;
    stacked = NULL;
    stacked_ransac = NULL;
    H = NULL;
    xformed = NULL;

    verticalStackFlag = false;//显示匹配结果的合成图像默认是横向排列
}

void SiftMatch::open_image()
{
    img1 = cvLoadImage(name1.c_str());
    img2 = cvLoadImage(name2.c_str());
}

void SiftMatch::detect_features()
{
    img1_Feat = cvCloneImage(img1);//复制图1，深拷贝，用来画特征点
    img2_Feat = cvCloneImage(img2);//复制图2，深拷贝，用来画特征点

    //默认提取的是LOWE格式的SIFT特征点
    //提取并显示第1幅图片上的特征点
    n1 = sift_features( img1, &feat1 );//检测图1中的SIFT特征点,n1是图1的特征点个数
    export_features("feature1.txt",feat1,n1);//将特征向量数据写入到文件
    draw_features( img1_Feat, feat1, n1 );//画出特征点

    //提取并显示第2幅图片上的特征点
    n2 = sift_features( img2, &feat2 );//检测图2中的SIFT特征点，n2是图2的特征点个数
    export_features("feature2.txt",feat2,n2);//将特征向量数据写入到文件
    draw_features( img2_Feat, feat2, n2 );//画出特征点
#ifdef DISPLAY
    cvNamedWindow(IMG2_FEAT);//创建窗口
    cvShowImage(IMG2_FEAT,img2_Feat);//显示
    cvWaitKey();
#endif
}


void SiftMatch::match_features()
{
    //将2幅图片合成1幅图片,img1在左，img2在右
    stacked = stack_imgs_horizontal(img1, img2);//合成图像，显示经距离比值法筛选后的匹配结果

    //根据图1的特征点集feat1建立k-d树，返回k-d树根给kd_root
    kd_root = kdtree_build( feat1, n1 );

    Point pt1,pt2;//连线的两个端点
    double d0,d1;//feat2中每个特征点到最近邻和次近邻的距离
    int matchNum = 0;//经距离比值法筛选后的匹配点对的个数

    //遍历特征点集feat2，针对feat2中每个特征点feat，选取符合距离比值条件的匹配点，放到feat的fwd_match域中
    for(int i = 0; i < n2; i++ )
    {
        feat = feat2+i;//第i个特征点的指针
        //在kd_root中搜索目标点feat的2个最近邻点，存放在nbrs中，返回实际找到的近邻点个数
        int k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
        if( k == 2 )
        {
            d0 = descr_dist_sq( feat, nbrs[0] );//feat与最近邻点的距离的平方
            d1 = descr_dist_sq( feat, nbrs[1] );//feat与次近邻点的距离的平方
            //若d0和d1的比值小于阈值NN_SQ_DIST_RATIO_THR，则接受此匹配，否则剔除
            if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
            {   //将目标点feat和最近邻点作为匹配点对
                pt2 = Point( cvRound( feat->x ), cvRound( feat->y ) );//图2中点的坐标
                pt1 = Point( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );//图1中点的坐标(feat的最近邻点)
                if(verticalStackFlag)//垂直排列
                    pt2.y += img1->height;//由于两幅图是上下排列的，pt2的纵坐标加上图1的高度，作为连线的终点
                else
                    pt2.x += img1->width;//由于两幅图是左右排列的，pt2的横坐标加上图1的宽度，作为连线的终点
                cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//画出连线
                matchNum++;//统计匹配点对的个数
                feat2[i].fwd_match = nbrs[0];//使点feat的fwd_match域指向其对应的匹配点
            }
        }
        free( nbrs );//释放近邻数组
    }

    n_match = matchNum;

    cout <<"经距离比值法筛选后的匹配点对个数："<<matchNum<<endl;
#ifdef DISPLAY
    //显示并保存经距离比值法筛选后的匹配图
    cvNamedWindow(IMG_MATCH1);//创建窗口
    cvShowImage(IMG_MATCH1,stacked);//显示
#endif
    //保存匹配图
    string name_match_DistRatio = name1;//文件名，原文件名去掉序号后加"_match_DistRatio"
    //cvSaveImage(name_match_DistRatio.replace( name_match_DistRatio.lastIndexOf(".",-1)-1 , 1 , "_match_DistRatio").toAscii().data(),stacked);

    //利用RANSAC算法筛选匹配点,计算变换矩阵H，
    //无论img1和img2的左右顺序，H永远是将feat2中的特征点变换为其匹配点，即将img2中的点变换为img1中的对应点
    H = ransac_xform(feat2,n2,FEATURE_FWD_MATCH,lsq_homog,4, 0.1,homog_xfer_err,30.0,&inliers,&n_inliers);

    for( int i = 0; i < 3; i ++)
        printf("H  %f %f %f\n", H->data.db[i * 3 + 0],H->data.db[i * 3 + 1],H->data.db[i * 3 + 2]);

    //特征点尺度归一化处理，以提高特征点匹配的成功率
    struct feature *featN;
    feat_normalization(feat2, &featN, n2);
    H = ransac_xform(featN,n2,FEATURE_FWD_MATCH,lsq_homog,4,0.1,homog_xfer_err,0.5,&inliers1,&n_inliers1);

    double s1[9] = {scale1, 0, -scale1 * point_average1.x,
        0, scale1, -scale1 * point_average1.y,
        0, 0, 1};
    T1 = cvMat(3, 3, CV_64F, s1);
    double s2[9] = {scale2, 0, -scale2 * point_average2.x,
        0, scale2, -scale2 * point_average2.y,
        0, 0, 1};
    T2 = cvMat(3, 3, CV_64F, s2);

    for( int i = 0; i < 3; i ++)
        printf("HH  %f %f %f\n", H->data.db[i * 3 + 0],H->data.db[i * 3 + 1],H->data.db[i * 3 + 2]);
    for( int i = 0; i < 3; i ++)
        printf("T1  %f %f %f\n", T1.data.db[i * 3 + 0],T1.data.db[i * 3 + 1],T1.data.db[i * 3 + 2]);
    for( int i = 0; i < 3; i ++)
        printf("T2  %f %f %f\n", T2.data.db[i * 3 + 0],T2.data.db[i * 3 + 1],T2.data.db[i * 3 + 2]);

    double t1[3][3], t2[3][3], t3[3][3];
    CvMat A,B,C;
    A = cvMat(3, 3, CV_64F, t1);
    B = cvMat(3, 3, CV_64F, t2);
    C = cvMat(3, 3, CV_64F, t3);
    cvInvert(&T1, &A);
    for( int i = 0; i < 3; i ++)
        printf("A  %f %f %f\n", A.data.db[i * 3 + 0],A.data.db[i * 3 + 1],A.data.db[i * 3 + 2]);
    cvMatMul(&A, H, &B);
    cvMatMul(&B, &T2, &A);
    for( int i = 0; i < 3; i ++)
        printf("A  %f %f %f\n", A.data.db[i * 3 + 0],A.data.db[i * 3 + 1],A.data.db[i * 3 + 2]);
    cvScale( &A, H, 1./A.data.db[8]);

    for( int i = 0; i < 3; i ++)
        printf("H1  %f %f %f\n", H->data.db[i * 3 + 0],H->data.db[i * 3 + 1],H->data.db[i * 3 + 2]);



    //若能成功计算出变换矩阵，即两幅图中有共同区域
    if( H )
    {
        cout<<"经RANSAC算法筛选后的匹配点对个数："<<n_inliers<<endl;
        cout<<"经RANSAC算法筛选归一化后的匹配点对个数："<<n_inliers1<<endl;
        //输出H矩阵
        for(int i=0;i<3;i++)
            cout<<cvmGet(H,i,0)<<cvmGet(H,i,1)<<cvmGet(H,i,2)<<endl;

        if(verticalStackFlag)//将2幅图片合成1幅图片,img1在上，img2在下
            stacked_ransac = stack_imgs( img1, img2 );//合成图像，显示经RANSAC算法筛选后的匹配结果
        else//将2幅图片合成1幅图片,img1在左，img2在右
            stacked_ransac = stack_imgs_horizontal(img1, img2);//合成图像，显示经RANSAC算法筛选后的匹配结果

        int invertNum = 0;//统计pt2.x > pt1.x的匹配点对的个数，来判断img1中是否右图

        //遍历经RANSAC算法筛选后的特征点集合inliers，找到每个特征点的匹配点，画出连线
        for(int i=0; i<n_inliers; i++)
        {
            feat = inliers[i];//第i个特征点
            pt2 = Point(cvRound(feat->x), cvRound(feat->y));//图2中点的坐标
            pt1 = Point(cvRound(feat->fwd_match->x), cvRound(feat->fwd_match->y));//图1中点的坐标(feat的匹配点)

            //统计匹配点的左右位置关系，来判断图1和图2的左右位置关系
            if(pt2.x > pt1.x)
                invertNum++;

            if(verticalStackFlag)//垂直排列
                pt2.y += img1->height;//由于两幅图是上下排列的，pt2的纵坐标加上图1的高度，作为连线的终点
            else//水平排列
                pt2.x += img1->width;//由于两幅图是左右排列的，pt2的横坐标加上图1的宽度，作为连线的终点
            cvLine(stacked_ransac,pt1,pt2,CV_RGB(255,0,255),1,8,0);//在匹配图上画出连线
        }
#ifdef DISPLAY
        cvNamedWindow(IMG_MATCH2);//创建窗口
        cvShowImage(IMG_MATCH2,stacked_ransac);//显示经RANSAC算法筛选后的匹配图
#endif
        /*程序中计算出的变换矩阵H用来将img2中的点变换为img1中的点，正常情况下img1应该是左图，img2应该是右图。
          此时img2中的点pt2和img1中的对应点pt1的x坐标的关系基本都是：pt2.x < pt1.x
          若用户打开的img1是右图，img2是左图，则img2中的点pt2和img1中的对应点pt1的x坐标的关系基本都是：pt2.x > pt1.x
          所以通过统计对应点变换前后x坐标大小关系，可以知道img1是不是右图。
          如果img1是右图，将img1中的匹配点经H的逆阵H_IVT变换后可得到img2中的匹配点*/

        //若pt2.x > pt1.x的点的个数大于内点个数的80%，则认定img1中是右图
        if(invertNum > n_inliers * 0.8)
        {
            cout<<"img1中是右图"<<endl;
            CvMat * H_IVT = cvCreateMat(3, 3, CV_64FC1);//变换矩阵的逆矩阵
            //求H的逆阵H_IVT时，若成功求出，返回非零值
            if( cvInvert(H,H_IVT) )
            {
//                //输出H_IVT
//                for(int i=0;i<3;i++)
//                    qDebug()<<cvmGet(H_IVT,i,0)<<cvmGet(H_IVT,i,1)<<cvmGet(H_IVT,i,2);
                cvReleaseMat(&H);//释放变换矩阵H，因为用不到了
                H = cvCloneMat(H_IVT);//将H的逆阵H_IVT中的数据拷贝到H中
                cvReleaseMat(&H_IVT);//释放逆阵H_IVT
                //将img1和img2对调
                IplImage * temp = img2;
                img2 = img1;
                img1 = temp;
            }
            else//H不可逆时，返回0
            {
                cvReleaseMat(&H_IVT);//释放逆阵H_IVT
            }
        }
    }
    else //无法计算出变换矩阵，即两幅图中没有重合区域
    {
        printf("两图中无公共区域\n");
    }
#ifdef DISPLAY
    cvWaitKey();
#endif
}


void SiftMatch::match_image()
{
    //若能成功计算出变换矩阵，即两幅图中有共同区域，才可以进行全景拼接
    if(H)
    {
        //拼接图像，img1是左图，img2是右图
        calc_four_corner();//计算图2的四个角经变换后的坐标
        printf("rightTop.x: %d rightBottom.x: %d\n");
        //为拼接结果图xformed分配空间,高度为图1图2高度的较小者，根据图2右上角和右下角变换后的点的位置决定拼接图的宽度
        xformed = cvCreateImage(cvSize(MIN(rightTop.x,rightBottom.x),MIN(img1->height,img2->height)),IPL_DEPTH_8U,3);
//        用变换矩阵H对右图img2做投影变换(变换后会有坐标右移)，结果放到xformed中
        cvWarpPerspective(img2,xformed,H,CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
#ifdef DISPLAY
        cvNamedWindow(IMG_MOSAIC_TEMP); //显示临时图,即只将图2变换后的图
        cvShowImage(IMG_MOSAIC_TEMP,xformed);
#endif
        //处理后的拼接图，克隆自xformed
        xformed_proc = cvCloneImage(xformed);

        //重叠区域左边的部分完全取自图1
        cvSetImageROI(img1,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(xformed,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvSetImageROI(xformed_proc,cvRect(0,0,MIN(leftTop.x,leftBottom.x),xformed_proc->height));
        cvAddWeighted(img1,1,xformed,0,0,xformed_proc);
        cvResetImageROI(img1);
        cvResetImageROI(xformed);
        cvResetImageROI(xformed_proc);
        cvNamedWindow(IMG_MOSAIC_BEFORE_FUSION);
        cvShowImage(IMG_MOSAIC_BEFORE_FUSION,xformed_proc);//显示融合之前的拼接图

//        采用加权平均的方法融合重叠区域
        int start = MIN(leftTop.x,leftBottom.x) ;//开始位置，即重叠区域的左边界
        double processWidth = img1->width - start;//重叠区域的宽度
        double alpha = 1;//img1中像素的权重
        for(int i=0; i<xformed_proc->height; i++)//遍历行
        {
            const uchar * pixel_img1 = ((uchar *)(img1->imageData + img1->widthStep * i));//img1中第i行数据的指针
            const uchar * pixel_xformed = ((uchar *)(xformed->imageData + xformed->widthStep * i));//xformed中第i行数据的指针
            uchar * pixel_xformed_proc = ((uchar *)(xformed_proc->imageData + xformed_proc->widthStep * i));//xformed_proc中第i行数据的指针
            for(int j=start; j<img1->width; j++)//遍历重叠区域的列
            {
                //如果遇到图像xformed中无像素的黑点，则完全拷贝图1中的数据
                if(pixel_xformed[j*3] < 50 && pixel_xformed[j*3+1] < 50 && pixel_xformed[j*3+2] < 50 )
                {
                    alpha = 1;
                }
                else
                {   //img1中像素的权重，与当前处理点距重叠区域左边界的距离成正比
                    alpha = (processWidth-(j-start)) / processWidth ;
                }
                pixel_xformed_proc[j*3] = pixel_img1[j*3] * alpha + pixel_xformed[j*3] * (1-alpha);//B通道
                pixel_xformed_proc[j*3+1] = pixel_img1[j*3+1] * alpha + pixel_xformed[j*3+1] * (1-alpha);//G通道
                pixel_xformed_proc[j*3+2] = pixel_img1[j*3+2] * alpha + pixel_xformed[j*3+2] * (1-alpha);//R通道
            }
        }
#ifdef DISPLAY
        cvNamedWindow(IMG_MOSAIC_PROC);//创建窗口
        cvShowImage(IMG_MOSAIC_PROC,xformed_proc);//显示处理后的拼接图
        cvWaitKey();
#endif
    }
}



//计算图2的四个角经矩阵H变换后的坐标
void SiftMatch::calc_four_corner()
{
    //计算图2的四个角经矩阵H变换后的坐标
    double v2[]={0,0,1};//左上角
    double v1[3];//变换后的坐标值
    CvMat V2 = cvMat(3,1,CV_64FC1,v2);
    CvMat V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);//矩阵乘法
    leftTop.x = cvRound(v1[0]/v1[2]);
    leftTop.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,leftTop,7,CV_RGB(255,0,0),2);

    //将v2中数据设为左下角坐标
    v2[0] = 0;
    v2[1] = img2->height;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    leftBottom.x = cvRound(v1[0]/v1[2]);
    leftBottom.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,leftBottom,7,CV_RGB(255,0,0),2);

    //将v2中数据设为右上角坐标
    v2[0] = img2->width;
    v2[1] = 0;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    rightTop.x = cvRound(v1[0]/v1[2]);
    rightTop.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,rightTop,7,CV_RGB(255,0,0),2);

    //将v2中数据设为右下角坐标
    v2[0] = img2->width;
    v2[1] = img2->height;
    V2 = cvMat(3,1,CV_64FC1,v2);
    V1 = cvMat(3,1,CV_64FC1,v1);
    cvGEMM(H,&V2,1,0,1,&V1);
    rightBottom.x = cvRound(v1[0]/v1[2]);
    rightBottom.y = cvRound(v1[1]/v1[2]);
    //cvCircle(xformed,rightBottom,7,CV_RGB(255,0,0),2);

}


void SiftMatch::feat_normalization(feature *featSrc, feature **featDis, int n)
{

      *featDis = (struct feature *)malloc(sizeof(feature) * n);

      int i;
      float sumI = 0, sumJ = 0, meanI = 0, meanJ = 0;
      float squareDist = 0, sumDist = 0, meanDist = 0;
      float scale = 0;
      float x, y, xx, yy, ww;
      // calculate the centroid
      int num = 0;

      //计算重心
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              sumI += featSrc[i].x;
              sumJ += featSrc[i].y;
              num ++;
          }
      }
      meanI = sumI / num;//重心点y坐标
      meanJ = sumJ / num;//重心点x坐标

      printf("num of matching points: %d\n", num);

      point_average2.x = meanI;
      point_average2.y = meanJ;
      // calculate the mean distance
      //计算平均距离
      //通过对每一个点到重心的距离求和然后求出平均距离
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              squareDist = pow(featSrc[i].x - meanI, 2)
                  + pow(featSrc[i].y - meanJ, 2);
              sumDist += pow((double)squareDist, 0.5);
          }
      }
      meanDist = sumDist / num;
      // set the similarity transform
      //计算相似变换，1/平均距离=尺度
      scale = pow(2, 0.5) / meanDist;
      scale2 = scale;
      double t[9] = {scale, 0, -scale * meanI,
          0, scale, -scale * meanJ,
          0, 0, 1};
     // T1 = cvMat(3, 3, CV_64F, t);
   //   Array2CvMat(t, T, 3, 3);
      // data normalization
      //数据归一化，通过计算
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              x = featSrc[i].x;
              y = featSrc[i].y;
              xx = t[0] * x + t[1] * y + t[2];
              yy = t[3] * x + t[4] * y + t[5];
              ww = t[6] * x + t[7] * y + t[8];
              xx = xx / ww;
              yy = yy / ww;
              (*featDis)[i].x = xx;
              (*featDis)[i].y = yy;
          }
      }

      sumI = 0, sumJ = 0, meanI = 0, meanJ = 0;
      squareDist = 0, sumDist = 0, meanDist = 0;
      scale = 0;
      // calculate the centroid
      //计算重心
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              sumI += featSrc[i].fwd_match->x;
              sumJ += featSrc[i].fwd_match->y;
          }
      }
      meanI = sumI / num;//重心点y坐标
      meanJ = sumJ / num;//重心点x坐标
      point_average1.x = meanI;
      point_average1.y = meanJ;
      // calculate the mean distance
      //计算平均距离
      //通过对每一个点到重心的距离求和然后求出平均距离
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              squareDist = pow(featSrc[i].fwd_match->x - meanI, 2)
                  + pow(featSrc[i].fwd_match->y - meanJ, 2);
              sumDist += pow((double)squareDist, 0.5);
          }
      }
      meanDist = sumDist / num;
      // set the similarity transform
      //计算相似变换，1/平均距离=尺度
      scale = pow(2, 0.5) / meanDist;
      scale1 = scale;
      double t1[9] = {scale, 0, -scale * meanI,
          0, scale, -scale * meanJ,
          0, 0, 1};
   //   T2 = cvMat(3, 3, CV_64F, t1);
   //   Array2CvMat(t, T, 3, 3);
      // data normalization
      //数据归一化，通过计算
      for(i = 0; i < n; i++){
          if(featSrc[i].fwd_match != NULL)
          {
              feature *tmp = (struct feature *)malloc(sizeof(feature));
              (*featDis)[i].fwd_match = tmp;
              x = featSrc[i].fwd_match->x;
              y = featSrc[i].fwd_match->y;
              xx = t1[0] * x + t1[1] * y + t1[2];
              yy = t1[3] * x + t1[4] * y + t1[5];
              ww = t1[6] * x + t1[7] * y + t1[8];
              xx = xx / ww;
              yy = yy / ww;
              (*featDis)[i].fwd_match->x = xx;
              (*featDis)[i].fwd_match->y = yy;
          }
      }
}
