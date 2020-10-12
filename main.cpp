#include <opencv2/opencv.hpp>
#include <iostream>
#include <errno.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdlib.h>
using namespace cv;
using namespace std;
void line_detector(int area,int left,int top,int width,int height);
void crosswalk_detector(int area,int left,int top,int width,int height);
void signal_detector(int area,int left,int top,int width,int height);
void line_follower(void);
void cross_standby(void);
void signal_catch(void);
void distance_calc(void);
void polygon_detect(Mat& image);
void setLabel(Mat& image, string str, vector<Point> contour);
int signal_flag, red_flag, green_flag, blue_flag;
int crosswalk_flag=0;
int lines[2][10]={0,};
int pos_L=3, pos_R=3;
char direction;
int range_count = 0;
int fd;
int data;
int aa=0;
int pleft=0,pright=0,countl=0,countr=0;
char viewflag,goflag,relineflag,distflag;
int offset_L=159;
int offset_R=160;
int trig = 23 ;
int echo = 24 ;
int start_time, end_time ;
float dist ;
Mat img_frame, img_hsv;

Scalar red(0, 0, 255);
Scalar green(0, 255, 0);
Scalar blue(255, 70, 70);
Scalar yellow(0, 255, 255);
Scalar white(230,230,230);
Scalar magenta(255, 0, 255);
Scalar black(10,10,10);
int main()
{
	if (wiringPiSetup() == -1) exit(1) ;
	pinMode(trig, OUTPUT) ;
	pinMode(echo , INPUT) ;
	
	fd = serialOpen("/dev/ttyAMA0", 115200);

    if(fd < 0){

        fprintf(stderr, "Failed to open serial device\n");

    }
    red_flag=1;
	Mat rgb_color = Mat(1, 1, CV_8UC3, red);
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
	cap.set(3,320);
	cap.set(4,240);
	

	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}


	while (1)
	{	
		
		
		

  
		lineloop:
		if(green_flag==0)
		{
			if(red_flag==1)
			{
				red_flag=0;blue_flag=1;
			}
			else
			{
				red_flag=1;blue_flag=0;
			}
		}
		if(viewflag==2)
		{
			if(red_flag==1)
			{
				
				rgb_color = Mat(1, 1, CV_8UC3, red);
				}
			else if(green_flag==1)
				rgb_color = Mat(1, 1, CV_8UC3, green);
			else if(blue_flag==1)
				rgb_color = Mat(1, 1, CV_8UC3, blue);
		}
	

	cvtColor(rgb_color, hsv_color, COLOR_BGR2HSV);


	hue = (int)hsv_color.at<Vec3b>(0, 0)[0];
	saturation = (int)hsv_color.at<Vec3b>(0, 0)[1];
	value = (int)hsv_color.at<Vec3b>(0, 0)[2];


	


	low_hue = hue - 5;
	high_hue = hue + 5;

	

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


	

		
		// wait for a new frame from camera and store it into 'frame'
		cap.read(img_frame);

		// check if we succeeded
		if (img_frame.empty()) 
		{
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}


		//HSV로 변환
		cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
		if(viewflag==0)
		rectangle(img_hsv,Point(0,0),Point(320,180),Scalar(0,0,0),-1);//blind
		else if(viewflag==1)
		{
			rectangle(img_hsv,Point(0,0),Point(320,70),Scalar(0,0,0),-1);//blind
			//rectangle(img_hsv,Point(0,180),Point(320,240),Scalar(0,0,0),-1);//blind
		}
		else
			rectangle(img_hsv,Point(0,180),Point(320,240),Scalar(0,0,0),-1);//blind
		//rectangle(img_hsv,Point(0,160),Point(320,240),Scalar(0,0,0),-1);
		//rectangle(img_hsv,Point(130,80),Point(190,160),Scalar(0,0,0),-1);

		//지정한 HSV 범위를 이용하여 영상을 이진화
		Mat img_mask1, img_mask2;
		if(viewflag!=2)
		inRange(img_hsv, Scalar(0, 0, 190), Scalar(180, 40, 255), img_mask1);
		else if(viewflag==2)
		{
			if(red_flag==1)
			{
				inRange(img_hsv, Scalar(170,100, 50), Scalar(180, 255, 255), img_mask1);
			}
			else
			{
				inRange(img_hsv, Scalar(low_hue1, 50, 50), Scalar(high_hue1, 255, 255), img_mask1);
				if (range_count == 2) 
				{
					inRange(img_hsv, Scalar(low_hue2, 50, 50), Scalar(high_hue2, 255, 255), img_mask2);
					img_mask1 |= img_mask2;
				}
			}
		}
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
		//dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//

		//morphological closing 영역의 구멍 메우기 
		//dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

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
		for (int j = 1; j < numOfLables; j++) 
		{  
			int area = stats.at<int>(j, CC_STAT_AREA);  
			int left = stats.at<int>(j, CC_STAT_LEFT);  
			int top  = stats.at<int>(j, CC_STAT_TOP);  
			int width = stats.at<int>(j, CC_STAT_WIDTH);  
			int height  = stats.at<int>(j, CC_STAT_HEIGHT);  
			
			if(viewflag==0)//line follow
				line_detector(area, left, top, width, height);
				
			else if(viewflag==1)
			{
				
				crosswalk_detector(area, left, top, width, height);
				
				
			}
			else
			{
				//polygon_detect(img_mask1);
				signal_detector(area, left, top, width, height);
			}
			//putText(img_color, to_string(j), Point(left+20,top+20), 
			 //                 FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2);  
			
		}
		
		
		if((aa<=5)&&(viewflag==1))
		{
			viewflag=2;
			cout<<"no"<<endl;
			goto lineloop;
		}
			
		if((goflag==1)&&(aa<4))
		{
			goflag=0;
			crosswalk_flag=0;
			viewflag=0;
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
		pleft=0;
		pright=320;
		if(viewflag==0)
		{
			line_follower();
		}
		if(viewflag==1)
		{
			cross_standby();
		}
		else
		{
			signal_catch();
			
			distance_calc();
			if(dist<=13)
			{
				serialPutchar(fd, 'x');
				//cout<<"too_close"<<endl;
				while(dist<=16)
				{
					distance_calc();
				}
				serialPutchar(fd, 'w');
				//cout<<"not_close"<<endl;
				
				
			}
		}
		if (waitKey(1) == 27)
			break;
			//delay(10);
	}



	return 0;
}
void line_detector(int area,int left,int top,int width,int height)
{	//aa=0;
	//cout<<"linedetect"<<endl;
	if((area>200)&&(top+(height/2)>=140)&&(top+(height/2)<=240)&&(left+(width/2)>=1)&&(left+(width/2)<=319))
	{
		rectangle( img_frame, Point(left,top), Point(left+width,top+height),  
				   Scalar(0,0,255),1 );
		rectangle( img_frame, Point(left+(width/2)-2,top+(height/2)-2), Point(left+(width/2)+2,top+(height/2)+2),  
				   Scalar(0,255,0),1 );
		if(left+(width/2)<=(156+2*(pos_L+pos_R)))
		{
			lines[0][aa]=left+(width/2);cout<<lines[0][aa]<<endl;
		}
		else if(left+(width/2)>156+2*(pos_L+pos_R))
		{
			lines[1][aa]=left+(width/2);cout<<lines[1][aa]<<endl;
		}
		aa++;
		if(aa>10)
		aa=9;
	}
}
void crosswalk_detector(int area,int left,int top,int width,int height)
{
	//cout<<"cross? : ";
	if(goflag==0)
	{
		if((area>200)&&(top+(height/2)>=140))
		{
			rectangle( img_frame, Point(left,top), Point(left+width,top+height),  
					   Scalar(0,0,255),1 );
			rectangle( img_frame, Point(left+(width/2)-2,top+(height/2)-2), Point(left+(width/2)+2,top+(height/2)+2),  
					   Scalar(0,255,0),1 );
			aa++;
			//cout<<"notyet"<<endl;
			if(aa>=5)
			{
				cout<<"----cross detect----"<<endl;
			crosswalk_flag=1;
			
			}
		}
		else
		{
			//cout<<"area : "<<area<<endl;
		//viewflag=0;
		//relineflag=1;
		
		}
		
	}
	else
	{
		//delay(3000);
		if((area>200)&&(top+(height/2)>=140))
		{
			rectangle( img_frame, Point(left,top), Point(left+width,top+height),  
					   Scalar(0,0,255),1 );
			rectangle( img_frame, Point(left+(width/2)-2,top+(height/2)-2), Point(left+(width/2)+2,top+(height/2)+2),  
					   Scalar(0,255,0),1 );
			aa++;
			//cout<<"passing"<<endl;
			if(aa>5)
			{
				cout<<"passing"<<endl;
			}
		
		}
	}
}

void signal_detector(int area,int left,int top,int width,int height)
{	//aa=0;
	//cout<<"linedetect"<<endl;
	if((area>500)&&(top+(height/2)>=0))
	{
		rectangle( img_frame, Point(left,top), Point(left+width,top+height),  
				   Scalar(0,0,255),1 );
		rectangle( img_frame, Point(left+(width/2)-2,top+(height/2)-2), Point(left+(width/2)+2,top+(height/2)+2),  
				   Scalar(0,255,0),1 );
		
		aa++;
		if(aa>0)
		signal_flag=1;
		

	}
	aa=0;
}

void line_follower(void)
{
	
	for(int c=0;c<=8;c++)
	{
		
		if(lines[0][c]!=0)
		{
			
			if(pleft<=lines[0][c])
				pleft=lines[0][c];
		}
		if(lines[1][c]!=0)
		{
			
			if(pright>=lines[1][c])
				pright=lines[1][c];
		}
		
	}
	
	memset((void*) lines,0, sizeof(int)*20 );


	if (!(countl==0))
	pleft/=countl;
	//else
	//left=140;
	if (!(countr==0))
	pright/=countr;
	//else
	//right=180;
	
	//cout<<viewflag<<" : "<<"left:"<<159-pleft<<", right"<<pright-160<<endl;
	
	
	if((offset_L-pleft)>145)
	{
		pos_L=1;
	}
	if(((offset_L-pleft)>=127)&&((offset_L-pleft)<=145))
	{
		pos_L=2;
	}
	if(((offset_L-pleft)>=86)&&((offset_L-pleft)<=126))
	{
		pos_L=3;
	}
	else if(((offset_L-pleft)>=66)&&((offset_L-pleft)<=85))
	{
		pos_L=4;
	}
	else if((offset_L-pleft)<=65)
	{
		pos_L=5;
	}
	else if((offset_L-pleft)==159)
		pos_L=0;
	if((pright-offset_R)>=136)//high
	{
		pos_R=5;
	}
	if(((pright-offset_R)>=116)&&((pright-offset_R)<=135))
	{
		pos_R=4;
	}
	if(((pright-offset_R)>=76)&&((pright-offset_R)<=115))
	{
		pos_R=3;
	}
	else if(((pright-offset_R)>=56)&&((pright-offset_R)<=75))
	{
		pos_R=2;
	}
	else if((pright-offset_R)<=55)//low
	{
		pos_R=1;
	}
	else if ((pright-offset_R)==160)
		pos_R=0;
	 
	if(pos_L==0)
	pos_L=pos_R;
	if(pos_R==0)
	pos_R=pos_L;
	direction=48+(pos_L+pos_R)/2;
	serialPutchar(fd, direction);
	//cout<<"direction : "<<direction<<endl;
	viewflag=1;
}

void cross_standby(void)
{
	if((crosswalk_flag==1)&&(goflag==0))
	{
		serialPutchar(fd, 'x');
		cout<<"cross_stop"<<endl;
		delay(5000);
		serialPutchar(fd, 'w');
		cout<<"run"<<endl;
		goflag=1;
	}
	
}
void signal_catch(void)
{	
	cout<<"rgb_status : "<<red_flag<<green_flag<<blue_flag<<endl;
	if((signal_flag==1)&&(red_flag==1))
	{
		serialPutchar(fd, 'x');
		cout<<"red_stop"<<endl;
		green_flag=1;
		red_flag=0;
		signal_flag=0;
		blue_flag=0;
	}
	else if((signal_flag==1)&&(green_flag==1))
	{
		serialPutchar(fd, 'w');
		cout<<"green_go"<<endl;
		signal_flag=0;
		viewflag=0;
		red_flag=1;
		green_flag=0;
	}
	else if((signal_flag==1)&&(blue_flag==1))
	{
		serialPutchar(fd, 's');
		cout<<"blue_slow"<<endl;
		signal_flag=0;
		viewflag=0;
		
	}
	if((signal_flag==0)&&((red_flag==1)||(blue_flag==1)))
		viewflag=0;

	
}
void polygon_detect(Mat& image)
{
	//contour를 찾는다.
	vector<vector<Point> > contours;
	findContours(image, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	//contour를 근사화한다.
	vector<Point2f> approx;
	

	for (size_t j = 0; j < contours.size(); j++)
	{
		approxPolyDP(Mat(contours[j]), approx, arcLength(Mat(contours[j]), true)*0.02, true);

		if (fabs(contourArea(Mat(approx))) > 1000 && fabs(contourArea(Mat(approx)))< 10000)  // <-----------------------   parameter 3
		{

			int size = approx.size();
			const char *name[]= { "none", "none", "none", "triangle", "quadrangle", "pentagon", "hexagon", "heptagon", "octagon", "nonagon", "decagon"  };

			switch (size){

				case 3: case 4: case 5:
				case 6: case 10:
					if (isContourConvex(Mat(approx))) { // convex 인지 검사 


						//Contour를 근사화한 직선을 그린다.
						if (size % 2 == 0) {
							line(img_frame, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 3);

							for (int k = 0; k < size - 1; k++)
								line(img_frame, approx[k], approx[k + 1], Scalar(0, 255, 0), 3);

							for (int k = 0; k < size; k++)
								circle(img_frame, approx[k], 3, Scalar(0, 0, 255));
						}
						else {
							line(img_frame, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 3);

							for (int k = 0; k < size - 1; k++)
								line(img_frame, approx[k], approx[k + 1], Scalar(0, 255, 0), 3);

							for (int k = 0; k < size; k++)
								circle(img_frame, approx[k], 3, Scalar(0, 0, 255));
						}


						//검출된 도형에 대한 라벨을 출력한다.

						setLabel(img_frame, name[size], contours[j]);
					

						

						

					}

					break;
					


				deafult:

					break;
			}


		}



	}
}
void setLabel(Mat& image, string str, vector<Point> contour)
{
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.5;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(str, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(image, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(200, 200, 200), FILLED);
	putText(image, str, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}
void distance_calc(void)
{
	if(distflag==3)
	{
		digitalWrite(trig, LOW) ;
		delay(20) ;
		digitalWrite(trig, HIGH) ;
		delayMicroseconds(10) ;
		digitalWrite(trig, LOW) ;
		while (digitalRead(echo) == 0) ;
		start_time = micros() ;
		while (digitalRead(echo) == 1);
		end_time = micros() ;
		dist = (end_time - start_time) / 29. / 2. ;
		printf("distance %.2f cm\n", dist) ;
		distflag=0;
	}
	else
	distflag++;
	
}
