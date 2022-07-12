#include <iostream>
#include <sys/stat.h>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main(){

    float squareWidth = 26;
    float squareHeight = 25;
    int boardWidth = 7;
    int boardHeight = 7;
    Size boardSize(boardWidth, boardHeight);
    vector<Point2f> pointsImg;
    vector<Point3f> pointsObj;

#pragma region READPARAMS
    cout << "Wir haben Spaß und geben Gas" << endl;

    Mat K, D;

    FileStorage fs("output.yml", FileStorage::READ);

    if(!fs.isOpened()){
        cout << "Output not found" << endl;
        return 0;
    }

    fs["K"] >> K;
    fs["D"] >> D;

    cout << K << endl << endl << D << endl;
#pragma endregion

#pragma region SETOBJECTPOINTS

    for(int i = -1 * boardWidth / 2; i < boardWidth - boardWidth / 2; i++){
        for(int j = 0; j < boardHeight; j++){
            Point3f p;
            p.x = (float)i * squareWidth;
            p.y = (float)j * squareHeight;
            p.z = 0;

            pointsObj.push_back(p);
        }
    }

#pragma endregion

#pragma region DETECTIMAGEPOINTS

    VideoCapture seq(0);
    if(!seq.isOpened()){
        cout << "No camera found" << endl;
        return 0;
    }

    Mat img, grayImg;
    seq >> img;

    cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);

    bool found = false;
    found = findChessboardCorners(grayImg, boardSize, pointsImg, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);
    if(found){
        cout << "Ecken wurden gefunden" << endl;
        cornerSubPix(grayImg, pointsImg, boardSize, Size(-1, -1),TermCriteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.1));
        drawChessboardCorners(img, boardSize, pointsImg, found);

        imshow("Schachbrett-Bunnt", img);
        waitKey(10000);
    }
    else{
        cout << "No corners found" << endl;
        return 0;
    }

#pragma endregion

    Mat rvec, tvec;
    solvePnP(pointsObj, pointsImg, K, D, rvec, tvec);

    cout << "Rot: " << rvec << " Trans: " << tvec << endl;

    FileStorage fsO("outputEx.yml", FileStorage::WRITE);
    fsO << "Trans" << tvec;
    fsO << "Rot" << rvec;

    return 0;
}