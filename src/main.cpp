#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono> 

using namespace cv;
using namespace std;
using namespace std::chrono; 



bool DetectSpots(const cv::Scalar color, const Mat& img,
     double threshold, double maxAreaSpot){
    
    Mat binaryImg(img.rows, img.cols, CV_8U, Scalar(0));

    std::vector<cv::Point> points;
    int refL = color[0]; int refA =  color[1]; int RefB =  color[2];
    cv::Mat labImg = img.clone();
    cvtColor(img,labImg,CV_RGB2Lab);
    for(int i=0; i<labImg.rows; i++){
        for(int j=0; j<labImg.cols; j++) {
            int l = labImg.at<cv::Vec3b>(i,j)[0];
            int a = labImg.at<cv::Vec3b>(i,j)[1];
            int b = labImg.at<cv::Vec3b>(i,j)[2];          
            double distnace = std::sqrt(pow(refL -l,2)  + pow(refA -a,2)
                  + pow(RefB -b,2) );

            if (distnace < threshold){
                circle(binaryImg, cv::Point(j,i),1, Scalar(255), -1, 8, 0);                
            }
           
        }
    }
    // imshow("binaryImg",binaryImg);
    // imshow("labImg",labImg);
    //         waitKey(1);
  
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_NONE, Point(0, 0));
    
    if (contours.size() == 1){
        double area = contourArea(contours[0]);
        if ( area < maxAreaSpot ){
            // drawContours(labImg, contours,0, Scalar(0,0,0),-1, 8, hierarchy, 0, Point());

            // imshow("labImg",labImg);
            // waitKey(1);
            return true;
        }
    
        
    }

    return false;     


}

enum DRINK_TYPE { BOTTLE, CAN, BOTTLE_BEER_1_5};

enum BOX_STATE { SEE_SPOT, SEE_BOTTLE, COUNT_BOTTLE_OR_CAN, PUSH_DRINK };

int main(int argc, char** argv)
{
    VideoCapture capture(3);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading video";

    BOX_STATE state = SEE_SPOT;
    auto start = high_resolution_clock::now();
    DRINK_TYPE currentType;
    int secondThreshold = 5;
    int secondPushing = 7;
    cv::Scalar color( 218, 99 , 182);
    bool stillPushing = false;

    for( ; ; )
    {
        capture >> frame;

        switch (state)
        {
            case SEE_SPOT:
            {               
                if( DetectSpots(color,frame, 25, 150) ){
                    putText(frame,"SPOT", cv::Point(100,100),
                        1,3,Scalar(0,0,255),3);
                    state = SEE_SPOT;
                } else {
                    state = SEE_BOTTLE;
                    start = high_resolution_clock::now();
                    break;
                }                
            }
            case SEE_BOTTLE:
            {   
                /// if doesnt recognize the sport 
                if( !DetectSpots(color,frame, 25, 150) ){
                    auto end = high_resolution_clock::now(); 
                    auto duration = duration_cast<seconds>(end - start); 
                    // if we pass more then X seconds, movnig to detect bottle
                    if ( duration.count() > secondThreshold){
                        putText(frame,"ALERT", cv::Point(100,100),1,3,Scalar(0,0,255),3);
                        state = COUNT_BOTTLE_OR_CAN;
                        break;
                        // imshow("alert",frame);
                        // waitKey(0);
                    } // we wait for the time will pass 
                    else {
                        state = SEE_BOTTLE;
                        break;
                    }                   
                } // we saw the spot ( it was noise) 
                else {
                    state = SEE_SPOT;
                    break;
                }               
            }

            case COUNT_BOTTLE_OR_CAN:
            {
                currentType = detectType(frame);
                countMoney(currentType);
                state = PUSH_DRINK;
                start = high_resolution_clock::now();
                break;
            }
            case PUSH_DRINK:
            {   
                if (!stillPushing){
                    pushDrinkdDirection(currentType); //thread
                    stillPushing = true;
                }
                auto end = high_resolution_clock::now(); 
                auto duration = duration_cast<seconds>(end - start); 
                if ( duration.count() > secondPushing){
                    
                    stillPushing = false;
                    state = SEE_SPOT;
                    
                    break;                   
                } else {
                    state = PUSH_DRINK;
                }
                break;
            }
            
            default:
                break;
        }
        
       
        imshow("frame",frame);
        waitKey(1);
       
    }
   
   return 0;
}