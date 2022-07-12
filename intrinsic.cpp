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

    cout << "Hallo OpenCV" << endl;

    Mat image, grayImg;
    //float squareSize = 28;
    //int boardWidth = 8;
    //int boardHeight = 6;
    float squareSize = 27;
    int boardWidth = 7;
    int boardHeight = 7;
    Size boardSize(boardWidth, boardHeight);
    vector<vector<Point2f> > pointsImg;
    vector<vector<Point3f> > pointsObj;

    int i = 0;

    VideoCapture seq(0);
    if(!seq.isOpened()){
        seq = VideoCapture("Images/ds1/%d.jpg");
    }
    //seq = VideoCapture("Images/ds2/%d.jpg");
    seq >> image;
    while(!image.empty() && i < 20){
        cvtColor(image, grayImg, cv::COLOR_BGR2GRAY);

        bool found = false;
        vector<Point2f> corners;
        found = findChessboardCorners(grayImg, boardSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);
        if(found){
            cout << "Ecken wurden gefunden" << endl;
            cornerSubPix(grayImg, corners, boardSize, Size(-1, -1),TermCriteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.1));
            drawChessboardCorners(image, boardSize, corners, found);
        }

        vector<Point3f> obj;
        for(int i = 0; i < boardHeight; i++){
            for(int j = 0; j < boardWidth; j++){
                obj.push_back(Point3f((float)j * squareSize, (float)i * squareSize, 0));
            }
        }

        i++;
        if(found){
            pointsImg.push_back(corners);
            pointsObj.push_back(obj);
            cout << "Bild-ID: " << i << endl;
        }

        imshow("Schachbrett-Bunnt", image);
        waitKey(5000);
        seq >> image;
    }

    Mat K;
    Mat D;
    vector<Mat> rvecs, tvecs;
    int flags = CALIB_FIX_K4 | CALIB_FIX_K5;

    double avrError = calibrateCamera(pointsObj, pointsImg, grayImg.size(), K, D, rvecs, tvecs, flags);

    FileStorage fs("output.yml", FileStorage::WRITE);
    fs << "K" << K;
    fs << "D" << D;
    fs << "board_width" << boardWidth;
    fs << "board_height" << boardHeight;
    fs << "square_size" << squareSize;
    fs << "Error" << avrError;

    cout    << "Groesse von ImgPoints:\t\t" << pointsImg.size() << endl
            << "Anzahl an eingelesenen Bildern:\t" << i << endl
            << "Fehler:\t\t\t" << avrError << endl;


    Mat Camera_Matrix = K;           // From calibration_data.xml
    Mat Distortion_Coefficients = D; // From calibration_data.xml

    image = cv::imread( "Images/ds2/14.jpg" );
    cv::Mat imageUndistorted; // Will be the undistorted version of the above image.

    undistort(image, imageUndistorted, Camera_Matrix, Distortion_Coefficients);
    imshow("Test Gerade", imageUndistorted);
    waitKey(0);
    
    return 0;

}