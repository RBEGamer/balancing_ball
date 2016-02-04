#include "opencv2/opencv.hpp"



#include <string>

#include <vector>

std::string intToString(int number){
    
    
    std::stringstream ss;
    ss << number;
    return ss.str();
}

using namespace cv;
const int frame_width = 500;
const int frame_height = 500;

const int MAX_NUM_OBJECTS=100;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 10*10;
const int MAX_OBJECT_AREA = 500*500/1.5;


void morphOps(Mat &thresh){
    
    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle
    
    Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
    
    erode(thresh,thresh,erodeElement);
    erode(thresh,thresh,erodeElement);
    
    
    dilate(thresh,thresh,dilateElement);
    dilate(thresh,thresh,dilateElement);
    
    
    
}
void on_trackbar( int, void* )
{//This function gets called whenever a
    // trackbar position is changed
    
    
    
    
    
}

int H_MIN = 0;
int H_MAX = 255;

int S_MIN = 0;
int S_MAX = 255;

int V_MIN = 0;
int V_MAX= 255;
void drawObject(int x, int y,Mat &frame){
    
    //use some of the openCV drawing functions to draw crosshairs
    //on your tracked image!
    
    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
    
    circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
        line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<frame_height)
        line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,frame_height),Scalar(0,255,0),2);
    if(x-25>0)
        line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<frame_width)
        line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(frame_width,y),Scalar(0,255,0),2);
    
    putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
    
}



void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){
    
    Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    std::vector<std::vector<Point> > contours;
    std::vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {
                
                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;
                
                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
                    x = moment.m10/area;
                    y = moment.m01/area;
                    objectFound = true;
                    refArea = area;
                }else objectFound = false;
                
                
            }
            //let user know you found an object
            if(objectFound ==true){
                //cvPutText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
                //draw object location on screen
                drawObject(x,y,cameraFeed);
            }
            
        } //cvPutText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
    }
}



int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    cap.set(CV_CAP_PROP_FRAME_WIDTH,frame_width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,frame_height);
    
    Mat curr_frame;
    
    
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    
    namedWindow("main",1);
    namedWindow("cf",1);
    cvCreateTrackbar2("H_MIN", "main", &H_MIN, H_MAX,on_trackbar);
    cvCreateTrackbar2("S_MIN", "main", &S_MIN, S_MAX,on_trackbar);
    cvCreateTrackbar2("V_MIN", "main", &V_MIN, V_MAX,on_trackbar);
    
    cvCreateTrackbar2("H_MAX", "main", &H_MAX, H_MAX,on_trackbar);
    cvCreateTrackbar2("S_MAX", "main", &S_MAX, S_MAX,on_trackbar);
    cvCreateTrackbar2("V_MAX", "main", &V_MAX, V_MAX,on_trackbar);
    
    int x=0, y=0;
    
    Mat curr_frame_range;
    
    unsigned int loops = 0;
    for(;;)
    {
        loops++;
        cap >> curr_frame; // get a new frame from camera
        
        cvtColor(curr_frame,curr_frame,CV_BGR2HSV_FULL);
        
        
        
        
        // cvInRange(&curr_frame,&hsv_min,&hsv_max,&curr_frame_range);
        
        
        
        
        Mat cf;
        
        //
        
        
        
        cv::Mat hsv_image;
        cv::cvtColor(curr_frame, hsv_image, cv::COLOR_BGR2HSV);
        
        // Threshold the HSV image, keep only the red pixels
        cv::Mat lower_red_hue_range;
        cv::Mat upper_red_hue_range;
        cv::inRange(hsv_image, cv::Scalar(H_MIN, S_MIN, V_MIN), cv::Scalar(H_MAX, S_MAX, V_MAX), lower_red_hue_range);
        //cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);
        
        morphOps(lower_red_hue_range);
        
        trackFilteredObject(x,y,lower_red_hue_range,lower_red_hue_range);
        
        imshow("view", curr_frame);
        
        imshow("main", lower_red_hue_range);
        
       // imshow("obj", cf);
        
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}