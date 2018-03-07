// opencvtest1.cpp : Defines the entry point for the console application.
//

#include"stdafx.h"//header for Visual Studio
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>//header for OpenCV core
#include <opencv2/highgui/highgui.hpp>//header for OpenCV UI
#include <iostream>//header for c++ IO
//set opencv and c++ namespaces
using namespace cv;
using namespace std;

String sample_pic_dir = "C:/Users/ARDI/source/repos/opencvtest1/opencvtest1/sample_pic/";
String output_pic_dir = "C:/Users/ARDI/source/repos/opencvtest1/opencvtest1/tmp/";

void open_image() {
	Mat image = imread(sample_pic_dir + "nasgor.jpg");
	if (!image.data) // check whether the image is found or not
	{
		cout << "Image is not found. Please write the file name and path location correctly." << endl;
	}
	namedWindow("lena", WINDOW_AUTOSIZE);
	imshow("lena", image);
}

void hist_equalized_image() {
	Mat image = imread(sample_pic_dir + "nasgor.jpg");

	Mat hist_equalized_image = imread(sample_pic_dir + "nasgor.jpg", 0);
	cvtColor(image, hist_equalized_image, CV_BGR2YCrCb);

	vector<Mat> vec_channels;
	split(hist_equalized_image, vec_channels);

	equalizeHist(vec_channels[0], vec_channels[0]);

	merge(vec_channels, hist_equalized_image);

	cvtColor(hist_equalized_image, hist_equalized_image, CV_YCrCb2BGR);

	String windowNameOfOriginalImage = "Original Image";
	String windowNameOfHistogramEqualized = "Histogram Equalized Color Image";

	// Create windows with the above names
	namedWindow(windowNameOfOriginalImage, WINDOW_NORMAL);
	namedWindow(windowNameOfHistogramEqualized, WINDOW_NORMAL);

	// Show images inside the created windows
	imshow(windowNameOfOriginalImage, image);
	imshow(windowNameOfHistogramEqualized, hist_equalized_image);
}

void otsu() {
	Mat im_gray;

	Mat im_rgb = imread(sample_pic_dir + "buku/a.jpg");
	cvtColor(im_rgb, im_gray, CV_RGB2GRAY);

	Mat img_bw;

	threshold(im_gray, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	int height = img_bw.rows;
	int width = img_bw.cols;

	Size smallsize(height/4, width/4);
	vector<Mat> partisi;
	
	for (int i = 0; i < height; i += smallsize.height) {
		for (int j = 0; j < width; j += smallsize.width) {
			cout << i << " " << j << endl;
			Rect rect = Rect(0, 0, width/4, height/4);
			imwrite(output_pic_dir + "buku/sub/" + i + "-" + j + ".jpg", Mat(img_bw , rect));
			//partisi.push_back( Mat(img_bw, rect) );
			
		}
	}

	getchar();
	
	//imwrite(output_pic_dir + "buku/a_otsu.jpg", img_bw);
}



int main()
{
	otsu();

	waitKey(0);
	
	destroyAllWindows();
	return 0;
}
