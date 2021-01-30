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

    while (true)
    {

        capture >> frame;
        switch (state)
        {
        case IDLE:
        {
            vector<cv::Rect> b_rects = bottleCountingManager.detectTool(frame);
            cv::rectangle(frame, bottleCountingManager.area_, Scalar(0, 0, 0), 2);

            if (b_rects.size() == 0)
            {
                putText(frame, "IDLE", cv::Point(100, 100),
                        1, 3, Scalar(0, 0, 255), 3);
                state = IDLE;
            }
            else
            {
                state = SEE_BOTTLE;
                putText(frame, "SEE_BOTTLE, SUM IS :" + to_string(bottleCountingManager.sumMoney_),
                        cv::Point(100, 100),
                        1, 3, Scalar(0, 0, 255), 3);
                start = high_resolution_clock::now();
                break;
            }
        }
        case SEE_BOTTLE:
        {
            vector<cv::Rect> b_rects = bottleCountingManager.detectTool(frame);
            cv::rectangle(frame, bottleCountingManager.area_, Scalar(0, 0, 0), 2);

            if (b_rects.size() > 0)
            {

                if (b_rects.size() > 0)
                {
                    countBottleWinner++;
                }

                bottleCountingManager.drawDetecetdRects(frame, b_rects, cv::Scalar(0, 255, 0));

                auto end = high_resolution_clock::now();
                auto duration = duration_cast<seconds>(end - start);
                // if we pass more then X seconds, movnig to detect bottle
                if (duration.count() > bottleCountingManager.secondThreshold_)
                {

                    type = BOTTLE;                   

                    countBottleWinner = 0;
                    countCanWinner = 0;

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
                countBottleWinner = 0;
                countCanWinner = 0;

                putText(frame, "IDLE, SUM IS :" + to_string(bottleCountingManager.sumMoney_), cv::Point(100, 100),
                        1, 3, Scalar(0, 0, 255), 3);
                state = IDLE;
                break;
            }
        }

        case COUNT_BOTTLE_OR_CAN:
        {
            bottleCountingManager.countMoney(type);
            state = IDLE; //PUSH_DRINK;
            start = high_resolution_clock::now();
            type = UNKNOWN;
            bottleCountingManager.sendSms();
            // putText(frame,"SUM IS :" + to_string(sumMoney), cv::Point(100,100),
            //             1,3,Scalar(0,0,255),3);
            break;
        }
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

        imshow("frame", frame);
        waitKey(1);
    }

    return 0;
}
