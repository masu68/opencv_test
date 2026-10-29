#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    // 1. 打开视频文件
    // 把这里的路径换成你实际的视频路径
    string video_path = "/home/majinfei/Videos/smartcar/test.mp4"; 
    VideoCapture cap(video_path);

    // 检查视频是否成功打开
    if (!cap.isOpened()) {
        cout << "无法打开视频文件，请检查路径！" << endl;
        return -1;
    }

    Mat frame, gray_frame;
    int frame_count = 0; // 记录处理了多少帧

    // 2. 循`环读取视频的每一帧
    while (cap.read(frame)) 
    {
        frame_count++;
        // 因为视频通常是彩色的，先转成灰度图
        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

        // ========================================================
        // 下面的处理和之前完全一样，只是作用在每一帧上
        // ========================================================
        
        // 第一步：跳采压缩（0.5倍）
        Mat compressed_img;
        resize(gray_frame, compressed_img, Size(), 0.5, 0.5, INTER_LINEAR);

        // 第二步：OTSU 二值化
        Mat binary_img;
        threshold(compressed_img, binary_img, 0, 255, THRESH_BINARY | THRESH_OTSU);

        // 第三步：形态学滤波
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
        Mat closed_img;
        morphologyEx(binary_img, closed_img, MORPH_CLOSE, kernel);

        // 第四步：八邻域寻线（逐行扫描）
        Mat result_img;
        cvtColor(closed_img, result_img, COLOR_GRAY2BGR);
        
        int height = closed_img.rows;
        int width = closed_img.cols;
        
        // 注意：每次循环都要重新初始化边界数组
        vector<int> left_border(height, -1);
        vector<int> right_border(height, -1);

        int start_y = height - 10; 
        int center_x = width / 2;
        
        int start_x_L = -1;
        int start_x_R = -1;

        // 扫线找起点
        int current_x = center_x;
        while(current_x > 0 && closed_img.at<uchar>(start_y, current_x) == 0) current_x++; 
        while(current_x > 0 && closed_img.at<uchar>(start_y, current_x) > 0) {
            start_x_L = current_x;
            current_x--;
        }

        current_x = center_x;
        while(current_x < width - 1 && closed_img.at<uchar>(start_y, current_x) == 0) current_x--;
        while(current_x < width - 1 && closed_img.at<uchar>(start_y, current_x) > 0) {
            start_x_R = current_x;
            current_x++;
        }

        // 追踪边界并画中线
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

                // 找左边界
                for (int dx = -2; dx <= 2; dx++) { 
                    int check_x = x_L + dx;
                    if (check_x >= 0 && check_x < width && closed_img.at<uchar>(y, check_x) > 0) {
                        x_L = check_x;
                        found_L = true;
                        break; 
                    }
                }

                // 找右边界
                for (int dx = 2; dx >= -2; dx--) { 
                    int check_x = x_R + dx;
                    if (check_x >= 0 && check_x < width && closed_img.at<uchar>(y, check_x) > 0) {
                        x_R = check_x;
                        found_R = true;
                        break;
                    }
                }

                if (found_L) left_border[y] = x_L;
                if (found_R) right_border[y] = x_R;

                if (!found_L && !found_R) break; 
            }

            // 画线（绿线边界，红线中线）
            for (int y = 0; y < height; y++) {
                if (left_border[y] != -1) circle(result_img, Point(left_border[y], y), 1, Scalar(0, 255, 0), -1);
                if (right_border[y] != -1) circle(result_img, Point(right_border[y], y), 1, Scalar(0, 255, 0), -1);
                
                if (left_border[y] != -1 && right_border[y] != -1) {
                    int mid_x = (left_border[y] + right_border[y]) / 2;
                    circle(result_img, Point(mid_x, y), 1, Scalar(0, 0, 255), -1); 
                }
            }
        }

        // ========================================================
        // 显示视频帧
        // ========================================================
        imshow("Original Video", frame);
        imshow("Tracing Result", result_img);

        // 3. 控制播放速度，按 'q' 或 'Esc' 退出
        // waitKey(1) 表示每帧等待 1 毫秒，数值越大播放越慢
        char key = waitKey(10); 
        if (key == 'q' || key == 27) { // 27 是 Esc 键的 ASCII 码
            break;
        }
    }

    // 4. 释放资源
    cap.release();
    destroyAllWindows();
    
    cout << "视频处理完毕，共处理 " << frame_count << " 帧。" << endl;
    return 0;
}