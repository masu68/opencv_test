#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
    VideoCapture cap("/home/majinfei/Videos/smartcar/test.mp4");
    if(!cap.isOpened())
    {
        std::cout<< "Error: Could not open video file." << std::endl;
        return -1;
    }
    Mat frame;
    while(true){
        cap >> frame;
        if(frame.empty()){
            break;}
        imshow("Video", frame);
    if(waitKey(30) == 27){
            break;}
    }
    cap.release();
    destroyAllWindows();
    return 0;
}