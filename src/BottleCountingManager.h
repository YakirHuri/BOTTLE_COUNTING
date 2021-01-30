
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
    COUNT_BOTTLE_OR_CAN,
    PUSH_DRINK
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
        //can
        // if (!can_cascade_.load("/home/yakir/bottle/images/negative/output/cascade.xml"))
        // {
        //     cout << "--(!)Error loading  cascade\n";
        // };

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
};
