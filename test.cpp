#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;

int main(){
	Mat imagen = imread("escena.png");
	CascadeClassifier cascade;
	cascade.load("cascada.xml");	
	Mat gray,small;
	cvtColor(imagen,gray,CV_BGR2GRAY);
   equalizeHist(gray,gray);
	resize(gray,small,Size(imagen.rows/1.5,imagen.cols/1.5));	
   if(cascade.empty())
   	std::cout << "empty\n";
   std::vector<Rect> faces;
   cascade.detectMultiScale(small,faces,1.2,0,0|CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING | CV_HAAR_FIND_BIGGEST_OBJECT,Size(20,20));
   std::cout << faces.size()<<std::endl;
	namedWindow("test");
	imshow("test",small);
	waitKey(0);
	return 0;
}
