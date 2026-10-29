#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    // 1. 图像读取
    Mat src = imread("/home/majinfei/Pictures/121.jpg", IMREAD_GRAYSCALE);
    if (src.empty()) {
        cout << "图片读取失败，请检查路径！" << endl;
        return -1;
    }

    // ========================================================
    // 【第一步：跳采压缩】
    // ========================================================
    Mat compressed_img;
    // 缩小到原来的 0.5 倍，使用 INTER_LINEAR（双线性插值）保证缩放后的平滑
    // 如果你觉得还不够快，可以改成 0.25 (即四分之一)
    resize(src, compressed_img, Size(), 0.5, 0.5, INTER_LINEAR);


    // ========================================================
    // 【第二步：OTSU 二值化】
    // ========================================================
    Mat binary_img;
    threshold(compressed_img, binary_img, 0, 255, THRESH_BINARY | THRESH_OTSU);


    // ========================================================
    // 【第三步：形态学滤波（开运算+闭运算）】
    // ========================================================
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat closed_img;
    morphologyEx(binary_img, closed_img, MORPH_CLOSE, kernel);
    // (如果需要去噪点，可以加上 MORPH_OPEN，这里为了代码简洁暂略)


    // ========================================================
    // 【第四步：八邻域寻线（逐行扫描法）】
    // ========================================================
    
    // 注意：这里的 width 和 height 是压缩后的尺寸！
    Mat result_img;
    cvtColor(closed_img, result_img, COLOR_GRAY2BGR);
    
    int height = closed_img.rows;
    int width = closed_img.cols;
    
    vector<int> left_border(height, -1);
    vector<int> right_border(height, -1);

    // 1. 找起始点 (从底部倒数第10行开始，注意不要写死，防止越界)
    int start_y = height - 10; 
    int center_x = width / 2;
    
    int start_x_L = -1;
    int start_x_R = -1;

    // 向左扫找左边界
    int current_x = center_x;
    while(current_x > 0 && closed_img.at<uchar>(start_y, current_x) == 0) current_x++; 
    while(current_x > 0 && closed_img.at<uchar>(start_y, current_x) > 0) {
        start_x_L = current_x;
        current_x--;
    }

    // 向右扫找右边界
    current_x = center_x;
    while(current_x < width - 1 && closed_img.at<uchar>(start_y, current_x) == 0) current_x--;
    while(current_x < width - 1 && closed_img.at<uchar>(start_y, current_x) > 0) {
        start_x_R = current_x;
        current_x++;
    }

    // 2 & 3. 分左右追踪，逐行向上爬
    if(start_x_L != -1 && start_x_R != -1) 
    {
        int x_L = start_x_L;
        int x_R = start_x_R;
        
        left_border[start_y] = x_L;
        right_border[start_y] = x_R;

        for (int y = start_y - 1; y >= 0; y--)
        {
            bool found_L = false;
            bool found_R = false;

            // 左边界追踪
            for (int dx = -2; dx <= 2; dx++) { 
                int check_x = x_L + dx;
                if (check_x >= 0 && check_x < width && closed_img.at<uchar>(y, check_x) > 0) {
                    x_L = check_x;
                    found_L = true;
                    break; 
                }
            }

            // 右边界追踪
            for (int dx = 2; dx >= -2; dx--) { 
                int check_x = x_R + dx;
                if (check_x >= 0 && check_x < width && closed_img.at<uchar>(y, check_x) > 0) {
                    x_R = check_x;
                    found_R = true;
                    break;
                }
            }

            // 记录边界点并画图
            if (found_L) {
                left_border[y] = x_L;
                circle(result_img, Point(x_L, y), 1, Scalar(0, 255, 0), -1);
            }
            if (found_R) {
                right_border[y] = x_R;
                circle(result_img, Point(x_R, y), 1, Scalar(0, 255, 0), -1);
            }

            if (!found_L && !found_R) break; // 全断线了，退出
        }

        // 4. 画中线（红色）
        for (int y = 0; y < height; y++) {
            if (left_border[y] != -1 && right_border[y] != -1) {
                int mid_x = (left_border[y] + right_border[y]) / 2;
                circle(result_img, Point(mid_x, y), 1, Scalar(0, 0, 255), -1); 
            }
        }
    } else {
        cout << "底部没有找到赛道边缘！" << endl;
    }

    // 显示
    imshow("1. Original", src);
    imshow("2. Compressed & Filtered", closed_img); // 压缩+二值化+滤波后的图 (尺寸变小了)
    imshow("3. Tracing Result", result_img);
    
    waitKey(0);
    return 0;
}