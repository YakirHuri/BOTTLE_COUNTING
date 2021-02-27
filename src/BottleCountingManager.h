
#include <chrono>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
// sudo apt-get install libboost1.62-*
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>          // Include for boost::split
#include <fstream>

using namespace std;
using namespace cv;
using namespace std::chrono;

enum DRINK_TYPE
{
    BOTTLE,
    CAN,
    BOTTLE_BEER_1_5,
    UNKNOWN
};

enum BOX_STATE
{
    IDLE,
    SEE_BOTTLE,
    SEE_CAN,
    COUNT_BOTTLE_OR_CAN,
    PUSH_DRINK,
    WAIT_FINISH
};

class BottleCountingManager
{

public:
    BottleCountingManager()
    {
        initParams();
        cout<<"sumMoney_ "<<sumMoney_<<endl;
        cout<<"area_ "<<area_<<endl;
    }

    void initParams() {
        //bottle
        if (!bottles_cascade_.load("../src/bottle_cascade.xml"))
        {
            cout << "--(!)Error loading  cascade\n";
        };
      

        // Create an input filestream
        std::ifstream myFile("../src/params.csv");

        // Make sure the file is open
        if (!myFile.is_open())
            throw std::runtime_error("Could not open file");

        // Helper vars
        std::string line;
      

        int count = 0;
        while ( myFile.good())
        {
            std::getline(myFile, line);
            std::vector<std::string> words;
            boost::split(words, line, boost::is_any_of(","), boost::token_compress_on);
            switch (count)
            {
                case 0:
                    videoPort_ = atoi(words[1].c_str());
                    break;
                case 1:
                    secondThreshold_ = atoi(words[1].c_str());
                    break;
                case 2:
                    phoneNumber_ = (words[1].c_str());
                    break;
                case 3:
                    area_ = cv::Rect((atoi(words[1].c_str())), (atoi(words[2].c_str())),
                        (atoi(words[3].c_str())), (atoi(words[4].c_str())));
                    break;
                case 4:
                    moneyForBottle_ = atof(words[1].c_str());            
                    break;
                case 5:
                    moneyForCan_ = atof(words[1].c_str());              
                    break;
                case 6:
                    moneyForBigBottle_ = atof(words[1].c_str());               
                    break;
               case 7:
                    motorLeftPin_ = atoi(words[1].c_str());               
                    break;
               case 8:
                    motorRightPin_ = atoi(words[1].c_str());               
                    break;            
            }           
            count++;            
        }
    }

    std::vector<Rect> detectTool(const Mat &frame) {

        Mat frame_gray;
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        std::vector<Rect> bottles;
        std::vector<Rect> newREctsBottles;

        bottles_cascade_.detectMultiScale(frame_gray, bottles, 1.1, 3, 0,
                                          cv::Size(50, 100), cv::Size(100, 300));

        for (size_t i = 0; i < bottles.size(); i++)
        {

            if (bottles[i].x >= area_.x && bottles[i].x + bottles[i].width <= area_.x + area_.width && bottles[i].y >= area_.y && bottles[i].y + bottles[i].height <= area_.y + area_.height)
            {

                newREctsBottles.push_back(bottles[i]);
            }
        }

        return newREctsBottles;
    }

    void drawDetecetdRects(Mat frame, vector<cv::Rect> rects, cv::Scalar s)
    {

        for (int i = 0; i < rects.size(); i++)
        {
            cv::rectangle(frame, rects[i], s, 2);
        }
    }

    void countMoney(DRINK_TYPE type)
    {

        sumMoney_ += 0.33;
    }

    void sendSms() {
        string command = 
            "python3 ../src/sendSms.py " + phoneNumber_ + " " + to_string(sumMoney_) ;
        
        bool res = system(command.c_str());
        cout<<" the command is "<<command<<" and the rest is "<<res<<endl;

    }
    
    void pushDrinkdDirection(DRINK_TYPE type){
       
          if (type ==   DRINK_TYPE::BOTTLE){
                string command = 
                "python ../src/motor.py " + std::to_string(motorLeftPin_);
                
                bool res = system(command.c_str());
                cout<<" pushing bottle with pin  "<<motorLeftPin_<<endl;
          }
          else
          {
                string command = 
                "python ../src/motor.py " + std::to_string(motorRightPin_);
                
                bool res = system(command.c_str());
                cout<<" pushing bottle with pi  "<<motorRightPin_<<endl;
          }
       
    }
    
    bool recognizeCan2(const Mat& frame, const cv::Rect& roi){

        Mat crop  = frame(roi);
        cvtColor(crop, crop, COLOR_BGR2GRAY);
        
        Canny(crop, crop, 30, 60, 3);
        
        vector<Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(crop, linesP, 1, CV_PI/180, 50, 50, 20 ); // runs the actual detection
        // Draw the lines
        cvtColor(crop, crop, COLOR_GRAY2BGR);
        
        vector<Vec4i> finalsLines;
        int minX = crop.cols;
        int maxX = 0;
        int indexMin = -1;
        int indexMax = -1;
        
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            cv::line( crop, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,255), 3, LINE_AA);

            if( abs(l[0] - l[2] ) < 5){
                int topletf = l[2];
                if( l[0] < l[2]){
                    topletf = l[0];
                }
                if(topletf < minX){
                    minX = topletf;
                    indexMin = i;
                }
                
                int topRight = l[2];
                if( l[0] < l[2]){
                    topRight = l[0];
                }
                if(topRight > maxX ){
                    maxX = topRight;
                    indexMax = i;
                }  

            }

        }
        
        if ( indexMin != -1 && indexMax != -1 && indexMax != indexMin ){
            Vec4i leftLine = linesP[indexMin];
            Vec4i rightLine = linesP[indexMax];
            
            int distX = abs(leftLine[0] - rightLine[0]);
            
            cv::Point topLeft =cv::Point(leftLine[2],leftLine[3]);
            if( leftLine[1] < leftLine[3] ){
                topLeft = cv::Point(leftLine[0],leftLine[1]);
            }
            cv::Point topRight =cv::Point(rightLine[2],rightLine[3]);
            if( rightLine[1] < rightLine[3] ){
                topRight = cv::Point(rightLine[0],rightLine[1]);
            }
            int disty = abs(topRight.y - topLeft.y);
            if( disty < 10  && distX > 60){
               //cout<<" disty "<<disty<<endl;
               //cout<<"  distX "<<distX<<endl;
               cv::line( crop, Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(0,255,0), 3, LINE_AA);
               cv::line( crop, Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0), 3, LINE_AA);
               circle(crop, topRight,3, Scalar(255,0,255),FILLED, 8,0);
               circle(crop, topLeft,3, Scalar(255,0,255),FILLED, 8,0);
               
               imshow("crop",crop);
               
               waitKey(1);
               return true;

            } 
            else
            {   
                cout<<"mmmmmmmmmmmmmmmmmm"<<endl;
                imshow("crop",crop);
               
               waitKey(1);
            }     
       
        }     
      

       return false;
    
    }
    
    bool recognizeCan(const Mat& frame, const cv::Rect& roi){

        Mat crop  = frame(roi);
        cvtColor(crop, crop, COLOR_BGR2GRAY);
        
        Canny(crop, crop, 30, 60, 3);
        
        Mat element = getStructuringElement( 1,
                       Size( 2*1 + 1, 2*1+1 ),
                       Point( 1, 1 ) );
        dilate( crop, crop, element );
        
        vector<vector<Point> >contours;
        vector<Vec4i>hierarchy;
        int savedContour = -1;
        double maxArea = 0.0;
        // Find the largest contour
        findContours(crop, contours, hierarchy,RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
        
        vector<vector<Point> > finalContours;
        for (int i = 0; i< contours.size(); i++)
        {
            double area = contourArea(contours[i]);

            if (area > maxArea)
            {
                maxArea = area;
                savedContour = i;

            }

        }
  
        if( maxArea > 12000)
        {  
               
           cv::Mat binarayImage(crop.rows, crop.cols,CV_8UC1, cv::Scalar(0));

           drawContours(binarayImage, contours, savedContour, Scalar(255), 2, 8);
            
           crop = binarayImage;
           cvtColor(crop, crop, COLOR_GRAY2BGR);
            
           vector<cv::Point> canContour = contours[savedContour];
           cv::Point topLeft(1000,1000);
           cv::Point topRight(0,0);
           std::sort(canContour.begin(),canContour.end(), [](const cv::Point &p1, const cv::Point &p2){
                return (p1.y < p2.y);});
            
            int distYthreshold = 10;
            int numOfPointsOnSameLine = 0;
            for(int i =0; i< canContour.size(); i++){
            
                int diffY = canContour[i].y - canContour[0].y;
                if (diffY < distYthreshold){
                   circle(crop, topRight,3, Scalar(0,255,0),FILLED, 8,0);
 
                   numOfPointsOnSameLine++;
                   if(canContour[i].x < topLeft.x){
                    topLeft = canContour[i];
                    
                   } 
                   if(canContour[i].x > topRight.x){
                    topRight = canContour[i];

                   }
                }
            }
            
           int minMaxDistX = 50;  
           int minNUmPOinstSameLine = 10;   
           int diffXLeftRight = topRight.x - topLeft.x;
           circle(crop, topLeft,3, Scalar(255,0,255),FILLED, 8,0);
           circle(crop, topRight,3, Scalar(255,0,0),FILLED, 8,0);
           cout<<"numOfPointsOnSameLine " <<numOfPointsOnSameLine<<" dist x "<<topRight.x - topLeft.x<<endl;


           imshow("crop",crop);
           waitKey(1);
           
           if ( diffXLeftRight > minMaxDistX && numOfPointsOnSameLine > minNUmPOinstSameLine){
                return true;
           }
           else
           {
               cout<<"nnnnnnnnnnnnnnnnnnnn"<<endl;
           }

           //bool result = foundTwoVerticalLines(crop);           
           //return result;          

        }
        
        
        return false;       
    
    }
    
    bool foundTwoVerticalLines(Mat crop){
        
        vector<Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(crop, linesP, 1, CV_PI/180,  50, 50, 10 ); // runs the actual detection
        // Draw the lines
        cvtColor(crop, crop, COLOR_GRAY2BGR);
            
        vector<Vec4i> finalsLines;
        int minX = crop.cols;
        int maxX = 0;
        int indexMin = -1;
        int indexMax = -1;
        
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            cv::line( crop, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,255), 3, LINE_AA);

            if( abs(l[0] - l[2] ) < 5){
                int topletf = l[2];
                if( l[0] < l[2]){
                    topletf = l[0];
                }
                if(topletf < minX){
                    minX = topletf;
                    indexMin = i;
                }
                
                int topRight = l[2];
                if( l[0] < l[2]){
                    topRight = l[0];
                }
                if(topRight > maxX ){
                    maxX = topRight;
                    indexMax = i;
                }  

            }

        }
        
        if ( indexMin != -1 && indexMax != -1 && indexMax != indexMin ){
            Vec4i leftLine = linesP[indexMin];
            Vec4i rightLine = linesP[indexMax];
            
            int distX = abs(leftLine[0] - rightLine[0]);
            
            cv::Point topLeft =cv::Point(leftLine[2],leftLine[3]);
            if( leftLine[1] < leftLine[3] ){
                topLeft = cv::Point(leftLine[0],leftLine[1]);
            }
            cv::Point topRight =cv::Point(rightLine[2],rightLine[3]);
            if( rightLine[1] < rightLine[3] ){
                topRight = cv::Point(rightLine[0],rightLine[1]);
            }
            int disty = abs(topRight.y - topLeft.y);
            if( disty < 10 && distX > 60){
               
               cv::line( crop, Point(leftLine[0], leftLine[1]), Point(leftLine[2], leftLine[3]), Scalar(0,255,0), 3, LINE_AA);
               cv::line( crop, Point(rightLine[0], rightLine[1]), Point(rightLine[2], rightLine[3]), Scalar(0,255,0), 3, LINE_AA);
               circle(crop, topRight,3, Scalar(255,0,255),FILLED, 8,0);
               circle(crop, topLeft,3, Scalar(255,0,255),FILLED, 8,0);
               
               imshow("crop",crop);
                cout<<"true"<<endl;
               waitKey(1);
               return true;

            } 
            else
            {   
                cout<<" disty "<<disty<<endl;
                cout<<"  distX "<<distX<<endl;
                cout<<"false"<<endl;
                imshow("crop",crop);
               
               waitKey(1);
            }     
       
        } 
        
        return false; 
    }

public:
    int videoPort_ = 0;

    int secondThreshold_ = 5;

    double sumMoney_ = 0.0;

    cv::Rect area_;

private:
    CascadeClassifier bottles_cascade_;
    CascadeClassifier can_cascade_;
    string phoneNumber_;
    double moneyForBottle_;
    double moneyForCan_;
    double moneyForBigBottle_;
    int motorLeftPin_;
    int motorRightPin_;
};
