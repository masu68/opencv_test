#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    // 1. 读取原图（建议直接以灰度图模式读取，省去转换步骤）
    // 记得把 "track.jpg" 换成你实际赛道图片的路径
    Mat src = imread("/home/majinfei/Pictures/121.jpg", IMREAD_GRAYSCALE); 
    if (src.empty())
    {
        cout << "无法读取图片，请检查路径！" << endl;
        return -1;
    }

    // 2. 图像预处理（非常重要）
    // 在真实赛道中，由于光照不均或有噪点，OTSU 可能会把一些反光的地方误判。
    // 用高斯模糊平滑一下，能极大提升 OTSU 的效果。
    Mat blurred;
    GaussianBlur(src, blurred, Size(5, 5), 0);

    // 3. 第二步：二值化 OTSU
    Mat binary_img;
    // 参数解释：
    // blurred: 输入图像
    // binary_img: 输出图像
    // 0: 设定的阈值（因为用了 THRESH_OTSU，OpenCV 会忽略这个 0，自动计算最优阈值）
    // 255: 超过阈值的像素被赋予的值（白色）
    // THRESH_BINARY | THRESH_OTSU: 二值化模式 + 自动寻找最优阈值算法
    threshold(blurred, binary_img, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // 4. 显示图像
    // 由于赛道通常是黑色背景白线，或者白底黑线，你可以看看OTSU的效果反了没有
    imshow("Original Image", src);
    imshow("OTSU Binary Image", binary_img);

    waitKey(0);
    return 0;
}