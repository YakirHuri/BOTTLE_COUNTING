
#include <chrono> 
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace std;
using namespace cv;
using namespace std::chrono; 



enum DRINK_TYPE { BOTTLE, CAN, BOTTLE_BEER_1_5};

enum BOX_STATE { IDLE, SEE_BOTTLE, COUNT_BOTTLE_OR_CAN, PUSH_DRINK };


std::vector<Rect> detectBottle(const Mat& frame, cv::Rect area, 
    CascadeClassifier bottles_cascade ) {

    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    //-- Detect faces
    std::vector<Rect> bottles;
    std::vector<Rect> newREctsBottles;

    bottles_cascade.detectMultiScale( frame_gray, bottles, 1.1 , 3, 0,  cv::Size(50,100),  cv::Size(100,300) );
    
    for ( size_t i = 0; i < bottles.size(); i++ ) {

        if( bottles[i].x >= area.x &&  bottles[i].x + bottles[i].width <=  area.x + area.width
            &&  bottles[i].y >= area.y &&  bottles[i].y + bottles[i].height <=  area.y + area.height){            
            
            newREctsBottles.push_back(bottles[i]);
        }
    }

    return newREctsBottles;      
    
}

void drawDetecetdRects(Mat frame, vector<cv::Rect> rects){

    for(int i =0; i < rects.size(); i++){
        cv::rectangle( frame, rects[i], Scalar(0,255,0),2 );
    }
}


int main(int argc, char** argv) {   
    
    CascadeClassifier bottles_cascade;
    VideoCapture capture(0);    

    if( !capture.isOpened() ){
        cout<<"Error when reading video"<<endl;
        return -1;
    }
    
    int secondThreshold = 5;
    BOX_STATE state = IDLE;
    auto start = high_resolution_clock::now();
  
    bool stillPushing = false;

     //-- 1. Load the cascades
    if( !bottles_cascade.load("/home/yakir/bottle/images/negative/output/cascade.xml"  )){
        cout << "--(!)Error loading face cascade\n";
        return -1;
    };
    
     cv::Rect area(200,0,150,400);
   
    
    Mat frame;

    while(true) {   
       
        capture >> frame;
        switch (state)
        {
            case IDLE:
            {   
               
                vector<cv::Rect> b_rects = detectBottle( frame, area, bottles_cascade);  
                cv::rectangle( frame, area, Scalar(0,0,0),2 );
          
                if( b_rects.size() == 0 ){
                    putText(frame,"IDLE", cv::Point(100,100),
                        1,3,Scalar(0,0,255),3);
                    state = IDLE;
                } else {
                    state = SEE_BOTTLE;
                    putText(frame,"SEE_BOTTLE", cv::Point(100,100),
                        1,3,Scalar(0,0,255),3);
                    start = high_resolution_clock::now();
                    break;
                }                
            }
            case SEE_BOTTLE:
            {   
                vector<cv::Rect> b_rects = detectBottle( frame, area, bottles_cascade);  
                cv::rectangle( frame, area, Scalar(0,0,0),2 );
                if( b_rects.size() > 0 ){
                    
                    drawDetecetdRects(frame,b_rects);
                    auto end = high_resolution_clock::now(); 
                    auto duration = duration_cast<seconds>(end - start); 
                    // if we pass more then X seconds, movnig to detect bottle
                    if ( duration.count() > secondThreshold){
                        putText(frame,"ALERT", cv::Point(100,100),1,3,Scalar(0,0,255),3);
                        state = COUNT_BOTTLE_OR_CAN;                       
                        imshow("COUNT_BOTTLE_OR_CAN",frame);
                        waitKey(0);
                        return -1;
                         break;
                    } // we wait for the time will pass 
                    else
                    {
                        putText(frame,"SEE_BOTTLE:" + to_string(duration.count()), cv::Point(100,100),
                            1,3,Scalar(0,0,255),3);
                        state = SEE_BOTTLE;
                        break;
                    }                   
                } // we saw nothing
                else 
                {   
                    putText(frame,"IDLE", cv::Point(100,100),
                        1,3,Scalar(0,0,255),3);
                    state = IDLE;
                    break;
                }               
            }

            // case COUNT_BOTTLE_OR_CAN:
            // {
            //     currentType = detectType(frame);
            //     countMoney(currentType);
            //     state = PUSH_DRINK;
            //     start = high_resolution_clock::now();
            //     break;
            // }
            // case PUSH_DRINK:
            // {   
            //     if (!stillPushing){
            //         pushDrinkdDirection(currentType); //thread
            //         stillPushing = true;
            //     }
            //     auto end = high_resolution_clock::now(); 
            //     auto duration = duration_cast<seconds>(end - start); 
            //     if ( duration.count() > secondPushing){
                    
            //         stillPushing = false;
            //         state = IDLE;
                    
            //         break;                   
            //     } else {
            //         state = PUSH_DRINK;
            //     }
            //     break;
            // }
            
            default:
                break;
        }   

        imshow("frame",frame);
        waitKey(1); 
       
    }
   
   return 0;
}



