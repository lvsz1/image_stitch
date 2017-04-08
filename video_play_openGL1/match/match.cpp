#include "match.h"
#include "capture/parameters.h"

//全景图像的宽度、高度
extern int panorama_width, panorama_height;
//原始表格的宽度、高度
extern int table_width, table_height;

//查找表格
struct Position *matchTable = NULL;


/**
 * @brief readWidhtHeight 读取全景图像宽高的函数
 * @param table_width     原始表格的宽度
 * @param table_height    原始表格的高度
 * @param panorama_width  开发板处理的全景图像的宽度
 * @param panorama_height 开发板处理的全景图像的高度
 */
void readWidhtHeight(int &table_width, int &table_height,
                            int &panorama_width, int &panorama_height)
{
    FILE *input = fopen("/home/vincent/matchTable.txt", "rb");

    fscanf(input, "%d %d ", &table_width, &table_height);
    panorama_width = table_width / WIDTH_INTERVAL;
    panorama_height = table_height / HEIGHT_INTERVAL;

    fclose(input);
}


/**
 * @brief loadTable 加载拼接的表格
 * @param name      路径名
 * @param width     表格的宽
 * @param height    表格的高
 * @return
 */
struct Position * loadTable(int &width, int &height)
{
    printf("waiting for loading table......\n");

    FILE *input = fopen("/home/vincent/matchTable.txt", "rb");
    fscanf(input, "%d %d ", &width, &height);

    int size = width * height;
    matchTable = (struct Position*)malloc(size * sizeof(struct Position));
    struct Position *p = matchTable;

    //表格中的数据格式，加载到内存时，需要格式转换(节省内存)
    int imageIndex1;  //查表图片索引值
    double x1;  //横坐标x1
    double y1;  //纵坐标y1
    int imageIndex2; //融合时使用
    double x2;
    double y2;
    int fusion; //是否需要融合
    double r1;
    double r2;

//    //c语言读取double，需采用%lf
    for(int i = 0; i < size; i ++){
        fscanf(input, "%d %lf %lf ", &imageIndex1, &x1, &y1);
        fscanf(input, "%d %lf %lf ", &imageIndex2, &x2, &y2);
        fscanf(input, "%d %lf %lf ", &fusion, &r1, &r2);

        p->imageIndex1 = (uchar)imageIndex1;
        p->x1 = (int)x1;
        p->y1 = (int)y1;
        p->imageIndex2 = (uchar)imageIndex2;
        p->x2 = (int)x2;
        p->y2 = (int)y2;
        p->fusion = (uchar)fusion;
        //硬盘存储的表格 r范围(0, 1),为了加快执行速度，把double转化为整数
        //加载后的值在(0,100)之间
        p->r1 = (uchar)(r1 * 100);
        p->r2 = (uchar)(r2 * 100);

        ++ p;
    }
    fclose(input);
    return matchTable;
}

/**
 * @brief imageReflect
 * @param leftData1     左侧图像1数据
 * @param leftData2     左侧图像2数据
 * @param forwardData   前侧图像数据
 * @param rightData1    右侧图像1数据
 * @param rightData2    右侧图像2数据
 * @param forbackData   后侧图像数据
 * @param dst           拼接图像
 */
void imageReflect(uchar* ptr0, uchar* ptr1, uchar* ptr2, uchar* ptr3,
                  uchar* ptr4, uchar* ptr5, uchar *dst)
{
    int srcStep = V4L2_WIDTH * 2;

    uchar *ptrTmp1, *ptrTmp2, *ptrDst;

    ptrDst = dst;
    for(int h = 0; h < panorama_height; h ++){
       struct Position *p = matchTable + h * WIDTH_INTERVAL * table_width;
       //这里w+=2是因为yuyv查表时，每次查找2个像素
       for(int w = 0; w < panorama_width; w += 2){

           //选择图片索引
           switch(p->imageIndex1){
           case 0:
               ptrTmp1 = ptr0;
               ptrTmp2 = ptr1;
               break;
           case 1:
               ptrTmp1 = ptr1;
               ptrTmp2 = ptr2;
               break;
           case 2:
               ptrTmp1 = ptr2;
               ptrTmp2 = ptr3;
               break;
           case 3:
               ptrTmp1 = ptr3;
               ptrTmp2 = ptr4;
               break;
           case 4:
               ptrTmp1 = ptr4;
               ptrTmp2 = ptr5;
               break;
           case 5:
               ptrTmp1 = ptr5;
               break;
           default:
               ptrTmp1 = ptr0;
               ptrTmp2 = ptr1;
               break;
           }

           if(p->x1 < V4L2_WIDTH && p->y1 < V4L2_HEIGHT){
               if(!p->fusion) //非融合
               {
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 0) = *(ptrTmp1 + p->y1 * srcStep + p->x1 * 2);
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 1) = *(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 1);
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 2) = *(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 2);
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 3) = *(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 3);

               }else{ //融合
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 0) = (*(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 0) * p->r1 + *(ptrTmp2 + p->y2 * srcStep + p->x2 * 2 + 0) * p->r2)/100;
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 1) = (*(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 1) * p->r1 + *(ptrTmp2 + p->y2 * srcStep + p->x2 * 2 + 1) * p->r2)/100;
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 2) = (*(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 2) * p->r1 + *(ptrTmp2 + p->y2 * srcStep + p->x2 * 2 + 2) * p->r2)/100;
                   *(ptrDst + h * panorama_width * 2 + w * 2 + 3) = (*(ptrTmp1 + p->y1 * srcStep + p->x1 * 2 + 3) * p->r1 + *(ptrTmp2 + p->y2 * srcStep + p->x2 * 2 + 3) * p->r2)/100;
               }
           }

           p += (WIDTH_INTERVAL*2);
       }

   }


}

