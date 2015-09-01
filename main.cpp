#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <opencv2/opencv.hpp>

#include "HSVColor.h"

using namespace cv;

VideoCapture CreateCapture();
void CreateWindows();
void CreateTrackbars(HSVColor* min, HSVColor* max, int* blur);
int GetBlur(int blurPos);
void OnMouseLeftClick(int event, int x, int y, int flags, void* data);
Point2d* GetPosition(const Mat* hsvImage);

const char MainWindowStr[] = "Main";
const char TrackbarWindowStr[] = "Trackbar";
const char ResultWindowStr[] = "Result";

const char HMinStr[] = "H min:";
const char HMaxStr[] = "H max:";
const char SMinStr[] = "S min:";
const char SMaxStr[] = "S max:";
const char VMinStr[] = "V min:";
const char VMaxStr[] = "V max:";
const char BlurStr[] = "Blur:";

const int HSVStep = 15;
const int BlurMax = 99;

int main(int argc, char** argv)
{
    HSVColor colorMin(0, 0, 0);
    HSVColor colorMax(255, 255, 255);

    int blurPos = 1;
    int blur = 1;

    VideoCapture capture = CreateCapture();
    CreateWindows();
    CreateTrackbars(&colorMin, &colorMax, &blurPos);

    Mat frame;
    Mat resultFrame;
    Point2d prevPosition(0, 0);
    Point2d* position = 0;

    setMouseCallback(MainWindowStr, OnMouseLeftClick, &frame);

    while (true)
    {
        capture >> frame;

        cvtColor(frame, resultFrame, COLOR_BGR2HSV);

        blur = GetBlur(blurPos);
        setTrackbarPos(BlurStr, TrackbarWindowStr, blur);
        if (blur > 1)
        {
            medianBlur(resultFrame, resultFrame, blur);
        }

        inRange(resultFrame, Scalar(colorMin.H, colorMin.S, colorMin.V),
            Scalar(colorMax.H, colorMax.S, colorMax.V), resultFrame);

        position = GetPosition(&resultFrame);
        if (position)
        {
            if (*position != prevPosition)
            {
                std::cout << "x: " << (int)position->x << ", y: " << (int)position->y << std::endl;
                prevPosition = *position;
            }

            delete position;
        }

        imshow(MainWindowStr, frame);
        imshow(ResultWindowStr, resultFrame);

        if (waitKey(30) == 27) // Esc
        {
            break;
        }
    }

    return 0;
}

VideoCapture CreateCapture()
{
    VideoCapture capture;
    capture.open(0);
    if (!capture.isOpened())
    {
        exit(1);
    }

    return capture;
}

void CreateWindows()
{
    namedWindow(MainWindowStr, WINDOW_AUTOSIZE);
    namedWindow(TrackbarWindowStr, WINDOW_NORMAL);
    namedWindow(ResultWindowStr, WINDOW_AUTOSIZE);
}

void CreateTrackbars(HSVColor* min, HSVColor* max, int* blur)
{
    createTrackbar(HMinStr, TrackbarWindowStr, &(min->H), max->H);
    createTrackbar(HMaxStr, TrackbarWindowStr, &(max->H), max->H);
    createTrackbar(SMinStr, TrackbarWindowStr, &(min->S), max->S);
    createTrackbar(SMaxStr, TrackbarWindowStr, &(max->S), max->S);
    createTrackbar(VMinStr, TrackbarWindowStr, &(min->V), max->V);
    createTrackbar(VMaxStr, TrackbarWindowStr, &(max->V), max->V);
    createTrackbar(BlurStr, TrackbarWindowStr, blur, BlurMax);
}

int GetBlur(int blurPos)
{
    if (blurPos <= 0)
    {
        return 1;
    }

    int result = blurPos;
    if (!(blurPos % 2))
    {
        result += 1;
    }

    return result > BlurMax ? BlurMax : result;
}

void OnMouseLeftClick(int event, int x, int y, int flags, void* data)
{
    if (flags == EVENT_LBUTTONDOWN)
    {
        Mat src = *(Mat*)data;
        Mat rgbPixel = src(Rect(x, y, 1, 1));
        Mat hsvPixel;
        cvtColor(rgbPixel, hsvPixel, COLOR_BGR2HSV);

        Vec3b pixel = hsvPixel.at<Vec3b>(0, 0);
        //printf("H: %d, S: %d, V: %d\n", (int)pixel[0], (int)pixel[1], (int)pixel[2]);

        setTrackbarPos(HMinStr, TrackbarWindowStr, pixel[0] - HSVStep < 0 ? 0 : pixel[0] - HSVStep);
        setTrackbarPos(HMaxStr, TrackbarWindowStr, pixel[0] + HSVStep > 255 ? 255 : pixel[0] + HSVStep);
        setTrackbarPos(SMinStr, TrackbarWindowStr, pixel[1] - HSVStep < 0 ? 0 : pixel[1] - HSVStep);
        setTrackbarPos(SMaxStr, TrackbarWindowStr, pixel[1] + HSVStep > 255 ? 255 : pixel[1] + HSVStep);
        setTrackbarPos(VMinStr, TrackbarWindowStr, pixel[2] - HSVStep < 0 ? 0 : pixel[2] - HSVStep);
        setTrackbarPos(VMaxStr, TrackbarWindowStr, pixel[2] + HSVStep > 255 ? 255 : pixel[2] + HSVStep);
    }
}

Point2d* GetPosition(const Mat* hsvImage)
{
    int sumX = 0;
    int sumY = 0;
    int count = 0;

    for (int y = 0; y < hsvImage->rows; ++y)
    {
        for (int x = 0; x < hsvImage->cols; ++x)
        {
            int pixel = hsvImage->at<uchar>(y, x);
            if (pixel == 255)
            {
                sumX += x;
                sumY += y;
                ++count;
            }
        }
    }

    if (count != 0)
    {
        return new Point2d(sumX / count, sumY / count);
    }

    return 0;
}
