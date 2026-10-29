#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
    Mat src = imread("/home/majinfei/Pictures/180.jpg");
    if (src.empty())
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    int new_rows = src.rows / 2;
    int new_cols = src.cols / 2;
    int channels = src.channels();
    Mat dist_resize;
    resize(src, dist_resize, Size(new_cols, new_rows), 0, 0, INTER_LINEAR);
    imshow("Original Image", src);
    imshow("Resized Image", dist_resize);
    waitKey(0);
    return 0;
}