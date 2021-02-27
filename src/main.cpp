#include "BottleCountingManager.h"




int main(int argc, char **argv)
{
    
    
    
    BottleCountingManager bottleCountingManager;
   
    VideoCapture capture(bottleCountingManager.videoPort_);

    if (!capture.isOpened())
    {
        cout << "Error when reading video" << endl;
        return -1;
    }

    BOX_STATE state = IDLE;
    auto start = high_resolution_clock::now();

    bool stillPushing = false;

    Mat frame;

    DRINK_TYPE type = UNKNOWN;
    int countBottleWinner = 0;
    int countCanWinner = 0;
    
    int frame_width = 640; 

    int frame_height = 480; 
    
    bool stillPushing_ = false;

    //VideoWriter video("/home/pi/bottle_ws/BOTTLE_COUNTING/src/outcpp.avi",0,5, cv::Size(frame_width,frame_height),true);
    ///////////////
    /*while(true){
        capture >> frame;
        cv::Rect area_ = cv::Rect(170,0,150,400);

        bottleCountingManager.recognizeCan(frame, area_);
    
    }*/
    ////////////
    while (true)
    {

        capture >> frame;
        switch (state)
        {
        case IDLE:
        {   
            start = high_resolution_clock::now();
            cout<<"IDLE"<<endl;
            vector<cv::Rect> b_rects = bottleCountingManager.detectTool(frame);
            state = IDLE;
            
            if (b_rects.size() == 0)
            {
                putText(frame, "IDLE", cv::Point(100, 100),
                        1, 3, Scalar(0, 0, 255), 3);
                state = IDLE;
                
                if ( bottleCountingManager.recognizeCan(frame, bottleCountingManager.area_) )
                {
                    state = SEE_CAN;
                    break;
                }
                else
                {
                     state = IDLE;
                    break;
                }
            }            
            else
            {
                state = SEE_BOTTLE;    
                        
                start = high_resolution_clock::now();
                break;
            }
        }
        case SEE_CAN:
        {
            cout<<"SEE_CAN"<<endl;
            
            bool isCan = bottleCountingManager.recognizeCan(frame, bottleCountingManager.area_); 
            vector<cv::Rect> b_rects = bottleCountingManager.detectTool(frame);

          
            if (isCan /*&& b_rects.size() == 0*/)
            {               

                auto end = high_resolution_clock::now();
                auto duration = duration_cast<seconds>(end - start);
                // if we pass more then X seconds, movnig to detect bottle
                if (duration.count() > 3)
                {

                    type = CAN;               

                    state = COUNT_BOTTLE_OR_CAN;

                    break;
                } // we wait for the time will pass
                else
                {
                     putText(frame, "SEE_CAN:" + to_string(duration.count()), cv::Point(100, 100),
                            1, 3, Scalar(0, 0, 255), 3);
                    state = SEE_CAN;
                    break;
                }
            }
            // we saw nothing
            else
            {    
                state = IDLE;
                break;
            }
        }
        case SEE_BOTTLE:
        {   
            cout<<"SEE_BOTTLE"<<endl;
            
            vector<cv::Rect> b_rects = bottleCountingManager.detectTool(frame);
            cv::rectangle(frame, bottleCountingManager.area_, Scalar(0, 0, 0), 2);
          
            if (b_rects.size() > 0)
            {
                
                bottleCountingManager.drawDetecetdRects(frame, b_rects, cv::Scalar(0, 255, 0));

                auto end = high_resolution_clock::now();
                auto duration = duration_cast<seconds>(end - start);
                // if we pass more then X seconds, movnig to detect bottle
                if (duration.count() > bottleCountingManager.secondThreshold_)
                {

                    type = BOTTLE;        

                    state = COUNT_BOTTLE_OR_CAN;

                    break;
                } // we wait for the time will pass
                else
                {
                    putText(frame, "SEE_BOTTLE:" + to_string(duration.count()), cv::Point(100, 100),
                            1, 3, Scalar(0, 0, 255), 3);
                    state = SEE_BOTTLE;
                    break;
                }
            }
            // we saw nothing
            else
            {
               
               
                state = IDLE;
                break;
            }
        }

        case COUNT_BOTTLE_OR_CAN:
        {   
            cout<<"COUNT_BOTTLE_OR_CAN"<<endl;

            bottleCountingManager.countMoney(type);
            
            // putText(frame,"SUM IS :" + to_string(sumMoney), cv::Point(100,100),
            //             1,3,Scalar(0,0,255),3);
            
            state = PUSH_DRINK; 
            break;
        }
         case PUSH_DRINK:
         {   
             cout<<"PUSH_DRINK"<<endl;

             DRINK_TYPE  currentType = type;//BOTTLE;   
            
             bottleCountingManager.pushDrinkdDirection(currentType); //thread
             start = high_resolution_clock::now();
              
              state = WAIT_FINISH;
             
             break;
         }
         case WAIT_FINISH:
         {  
             cout<<"WAIT_FINISH"<<endl;
             int secondPushing = 1;

             auto end = high_resolution_clock::now();
             auto duration = duration_cast<seconds>(end - start);
             
             //finish pushing
             if ( duration.count() > secondPushing){
                 cout<<"still puhsing "<<endl;
                 stillPushing_ = false;
                 state = IDLE;
                 break;
             } 
             else 
             {   
                 cout<<"WAIT_FINISH ..........."<<endl;
                 state = WAIT_FINISH;
             }
         }         

        default:
            break;
        }
        
        cv::rectangle(frame, bottleCountingManager.area_, Scalar(0, 0, 0), 2);
        putText(frame, "SUM IS :" + to_string(bottleCountingManager.sumMoney_), cv::Point(100, 300),
                        1, 3, Scalar(0, 0, 255), 3);
    
        //video.write(frame);
        imshow("frame", frame);
        char c = waitKey(1);
       
        /// finsih the program and sens sms msg
        if( c==27)
        {
            bottleCountingManager.sendMail();

            break;
        }
    }
    
     // video.release();


    return 0;
}
