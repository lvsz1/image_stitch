#ifndef MATCH_H
#define MATCH_H
#include <cv.h>
#include <highgui.h>

/* PC机建立的表格格式
struct Position{
    int imageIndex1;  //查表图片索引值
    double x1;  //横坐标x1
    double y1;  //纵坐标y1

    int imageIndex2; //融合时使用
    double x2;
    double y2;

    int fusion; //是否需要融合
    double r1;
    double r2;
};
*/

struct Position{
    uchar imageIndex1;  //查表图片索引值
    int x1;  //横坐标x1
    int y1;  //纵坐标y1

    uchar imageIndex2; //融合时使用
    int x2;
    int y2;

    uchar fusion; //是否需要融合
    uchar r1;
    uchar r2;
};


/**
 * @brief readWidhtHeight 读取全景图像宽高的函数
 * @param table_width     原始表格的宽度
 * @param table_height    原始表格的高度
 * @param panorama_width  开发板处理的全景图像的宽度
 * @param panorama_height 开发板处理的全景图像的高度
 */
void readWidhtHeight(int &table_width, int &table_height,
                            int &panorama_width, int &panorama_height);

/**
 * @brief loadTable 加载拼接的表格
 * @param name      路径名
 * @param width     表格的宽
 * @param height    表格的高
 * @return
 */
struct Position * loadTable(int &width, int &height);


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
                  uchar* ptr4, uchar* ptr5, uchar *dst);


#endif // MATCH_H
