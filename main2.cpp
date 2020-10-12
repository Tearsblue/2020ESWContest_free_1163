#include <opencv2/opencv.hpp>
#include <iostream>
#include <errno.h>
#include <wiringSerial.h>
using namespace cv;
using namespace std;

int main()
{
	int range_count = 0;
	int fd;
    int data;
    char direction;
    int lines[2][10]={0,};
    int aa=0;
    int left=0,right=0,countl=0,countr=0;
	Scalar red(0, 0, 255);
	Scalar blue(255, 0, 0);
	Scalar yellow(0, 255, 255);
	Scalar white(230,230,230);
	Scalar magenta(255, 0, 255);
	Scalar black(10,10,10);

	fd = serialOpen("/dev/ttyAMA0", 115200);

    if(fd < 0){

        fprintf(stderr, "Failed to open serial device\n");

    }
	Mat rgb_color = Mat(1, 1, CV_8UC3, white);
	Mat hsv_color;

	cvtColor(rgb_color, hsv_color, COLOR_BGR2HSV);


	int hue = (int)hsv_color.at<Vec3b>(0, 0)[0];
	int saturation = (int)hsv_color.at<Vec3b>(0, 0)[1];
	int value = (int)hsv_color.at<Vec3b>(0, 0)[2];


	cout << "hue = " << hue << endl;
	cout << "saturation = " << saturation << endl;
	cout << "value = " << value << endl;


	int low_hue = hue - 10;
	int high_hue = hue + 10;

	int low_hue1 = 0, low_hue2 = 0;
	int high_hue1 = 0, high_hue2 = 0;

	if (low_hue < 10 ) {
		range_count = 2;

		high_hue1 = 180;
		low_hue1 = low_hue + 180;
		high_hue2 = high_hue;
		low_hue2 = 0;
	}
	else if (high_hue > 170) {
		range_count = 2;

		high_hue1 = low_hue;
		low_hue1 = 180;
		high_hue2 = high_hue - 180;
		low_hue2 = 0;
	}
	
	if(1) {
		range_count = 1;

		low_hue1 = low_hue;
		high_hue1 = high_hue;
	}


	cout << low_hue1 << "  " << high_hue1 << endl;
	cout << low_hue2 << "  " << high_hue2 << endl;


	VideoCapture cap(-1);
	cap.set(3,240);
	cap.set(4,180);
	Mat img_frame, img_hsv;


	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}


	while (1)
	{
		// wait for a new frame from camera and store it into 'frame'
		cap.read(img_frame);

		// check if we succeeded
		if (img_frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}


		//HSV로 변환
		cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
		
		rectangle(img_hsv,Point(0,0),Point(240,60),Scalar(0,0,0),-1);
		rectangle(img_hsv,Point(0,160),Point(240,180),Scalar(0,0,0),-1);
		rectangle(img_hsv,Point(100,60),Point(140,120),Scalar(0,0,0),-1);

		//지정한 HSV 범위를 이용하여 영상을 이진화
		Mat img_mask1, img_mask2;
		inRange(img_hsv, Scalar(0, 0, 180), Scalar(180, 40, 255), img_mask1);
		//if (range_count == 2) {
		//	inRange(img_hsv, Scalar(0, 0, 180), Scalar(180, 40, 255), img_mask2);
		//	img_mask1 |= img_mask2;
		//}
		/*
		inRange(img_hsv, Scalar(low_hue1, 50, 50), Scalar(high_hue1, 255, 255), img_mask1);
		if (range_count == 2) {
			inRange(img_hsv, Scalar(low_hue2, 50, 50), Scalar(high_hue2, 255, 255), img_mask2);
			img_mask1 |= img_mask2;
		}
		*/

		//morphological opening 작은 점들을 제거 
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		//morphological closing 영역의 구멍 메우기 
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<Vec3f> circles;
		//HoughCircles(img_frame, circles, HOUGH_GRADIENT, 1,20,50,35,0,0);
		//라벨링 
		Mat img_labels, stats, centroids;
		int numOfLables = connectedComponentsWithStats(img_mask1, img_labels,
			stats, centroids, 8, CV_32S);


		//영역박스 그리기
		/*int max = -1, idx = 0;
		for (int j = 1; j < numOfLables; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			if (max < area)
			{
				max = area;
				idx = j;
			}
		}
		

		int left = stats.at<int>(idx, CC_STAT_LEFT);
		int top = stats.at<int>(idx, CC_STAT_TOP);
		int width = stats.at<int>(idx, CC_STAT_WIDTH);
		int height = stats.at<int>(idx, CC_STAT_HEIGHT);


		rectangle(img_frame, Point(left, top), Point(left + width, top + height),
			Scalar(0, 0, 255), 1);
*/		aa=0;
		for (int j = 1; j < numOfLables; j++) {  
        int area = stats.at<int>(j, CC_STAT_AREA);  
        int left = stats.at<int>(j, CC_STAT_LEFT);  
        int top  = stats.at<int>(j, CC_STAT_TOP);  
        int width = stats.at<int>(j, CC_STAT_WIDTH);  
        int height  = stats.at<int>(j, CC_STAT_HEIGHT);  
  
		if((area>300)&&(top+(height/2)>=60)&&(top+(height/2)<=120)){
			
        rectangle( img_frame, Point(left,top), Point(left+width,top+height),  
                   Scalar(0,0,255),1 );
        rectangle( img_frame, Point(left+(width/2)-2,top+(height/2)-2), Point(left+(width/2)+2,top+(height/2)+2),  
                   Scalar(0,255,0),1 );
		if(left+(width/2)<=100)
			lines[0][aa]=left+(width/2);
		else if(left+(width/2)>140)
			lines[1][aa]=left+(width/2);
		aa++;
		if(aa>10)
		aa=9;
          }
        //putText(img_color, to_string(j), Point(left+20,top+20), 
         //                 FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2);  
    }  
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        Point center(c[0], c[1]);
        int radius = c[2];
 
        circle( img_frame, center, radius, Scalar(0,255,0), 2);
        circle( img_frame, center, 2, Scalar(0,0,255), 3);
    }

		//imshow("이진화 영상", img_mask1);
		imshow("원본 영상", img_frame);
		countl=0;
		countr=0;
		left=0;
		right=0;
		
		for(int c=0;c<=8;c++)
		{
			
			if(lines[0][c]!=0)
			{
				left+=lines[0][c];
				countl++;
			}
			if(lines[1][c]!=0)
			{
				right+=lines[1][c];
				countr++;
			}
			
		}
		
		memset((void*) lines,0, sizeof(int)*20 );
		
		/*	for(int d=0;d<=8;d++)
		{
			lines[0][d]=0;
			lines[1][d]=0;
		}*/
		if (!(countl==0))
		left/=countl;
		else
		left=32;
		if (!(countr==0))
		right/=countr;
		else
		right=420;
		
		cout<<"left:"<<140-left<<", right"<<right-180<<endl;
		if((140-left)>(right-180))
		{
			direction='q';
			serialPutchar(fd, direction);
			//cout<<'e'<<endl;
		}
		else if((140-left)<(right-180))
		{
			direction='e';
			serialPutchar(fd, direction);
			//cout<<'q'<<endl;
		}
		 
		if (waitKey(1) == 27)
			break;
	}


	return 0;
}
