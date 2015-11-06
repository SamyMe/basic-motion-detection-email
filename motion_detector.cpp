#include<time.h>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include<opencv/cv.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void * send( void *){
        system("python sendmail.py");
        pthread_exit(0);
}

int main(){

    pthread_t sendEmail;

    cv::Size s;
    cvNamedWindow( "W1", CV_WINDOW_AUTOSIZE );
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(CV_CAP_ANY);
    Mat firstframe, frame, grey, diff;

    std::vector<std::vector<cv::Point> > contours;
    CvRect boundbox;

    firstframe = cvQueryFrame(capture);
    cvtColor(firstframe,firstframe,COLOR_BGR2GRAY);
    GaussianBlur(firstframe, firstframe,Size(21,21),0,0);    

    time_t t;
    time_t lastTime = 0;

    int detected = 0;
    int count = 1;

    while (1){
        time(&t);
        frame = cvQueryFrame(capture);
        cvtColor(frame,grey,COLOR_BGR2GRAY);
        GaussianBlur(grey, grey,Size(21,21),0,0);

        // La taille de l'image est de 640*480 
        //s = frame.size();
        //printf("%d  %d", s.height, s.width);

        absdiff(firstframe, grey, diff);
        threshold(diff, diff,25, 255, THRESH_BINARY);

        findContours(diff, contours, RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

        for( int i = 0; i< contours.size(); i++ )
            {   
            if (contourArea(contours[i]) < 100)
                break;
            
            detected=1;

            boundbox = boundingRect(contours[i]);
            rectangle(frame, boundbox, Scalar(0,0,255),2);
            }

        if(detected){
            time(&t);
            if(t>60+lastTime)
                {
                //imwrite("motion.jpg", frame);
                 
                if (count == 1)
                    imwrite( "motion1.jpg", frame);
                
                if (count == 2)
                    {
                    time(&lastTime);
                    imwrite( "motion2.jpg", frame);
                    pthread_create(&sendEmail,NULL, send, NULL);
                    count = 1;
                    }

                count++;
                }
                
        }

        imshow("W1",frame); 
        cvWaitKey(333);
    }

}

