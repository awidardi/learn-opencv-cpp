// opencvtest1.cpp : Defines the entry point for the console application.
//

#include"stdafx.h"//header for Visual Studio
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>//header for OpenCV core
#include <opencv2/highgui/highgui.hpp>//header for OpenCV UI
#include <iostream>//header for c++ IO
#include <fstream>
//set opencv and c++ namespaces
using namespace cv;
using namespace std;

String sample_pic_dir = "C:/Users/GP62MVR-7RFX/source/repos/opencvtest1/opencvtest1/sample_pic/";
//String sample_pic_dir = "C:/Users/ARDI/source/repos/opencvtest1/opencvtest1/sample_pic/";
String output_pic_dir = "C:/Users/GP62MVR-7RFX/source/repos/opencvtest1/opencvtest1/tmp/";
//String output_pic_dir = "C:/Users/ARDI/source/repos/opencvtest1/opencvtest1/tmp/";

int cnt[3500];
long long dpSigmaXSquare[3005];
long long dpSigmaX[3005];
long long dpSigmaY[3005];
long long dpSigmaXY[3005];

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

int countLostPixel(Mat image) {
	int cnt = 0;
	int height = image.rows;
	int width = image.cols;

	for (int i = 0;i < height - 1;i++) {
		for (int j = 0;j < width - 1;j++) {
			if ((int)image.at<uchar>(j, i) != 0 && (int)image.at<uchar>(j, i) != 255) {
				cnt++;
			}
		}
	}
	return cnt;
}

void printPixel(Mat image) {
	int height = image.rows;
	int width = image.cols;
	
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width; j++) {
			if (j != width - 1) cout << (int)image.at<uchar>(i, j) << " ";
			else cout << (int)image.at<uchar>(i, j) << "\n";
		}
	}
}

Mat changeVerticalBlackIntoWhite(Mat image, int x, int y, int len) {

	for (int i = 1;i <= len;i++) {
		image.at<uchar>(x + i - 1, y) = 255;
	}
	return image;
}

Mat changeVerticalWhiteIntoBlack(Mat image, int x, int y, int len) {

	for (int i = 1;i <= len;i++) {
		image.at<uchar>(x + i - 1, y) = 0;
	}
	return image;
}

void countBlackVerticalConsecutivePixels(int &width, int &height, int &maxi, int &maxj, int &maxlen, Mat &resultAfterPartialOtsu) {
	int tmplen;
	int tmpi = -1;
	int tmpj = -1;
	//0 black
	
	for (int j = 0;j < width;j++) {
		for (int i = 0;i < height;i++) {
			if (i == 0) {
				if (tmpi != -1 && tmpj != -1) {
					cnt[tmplen]++;
					if (maxlen < tmplen) {
						maxlen = tmplen;
						maxi = tmpi;
						maxj = tmpj;
					}
					tmplen = 0;
				}
				if (resultAfterPartialOtsu.at<uchar>(i, j) == 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
			}
			else {
				// before white current black
				if (resultAfterPartialOtsu.at<uchar>(i, j) == 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) != 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
				// before black current black
				else if (resultAfterPartialOtsu.at<uchar>(i, j) == 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) == 0) {
					tmplen++;
				}
				// before black current white
				else if (resultAfterPartialOtsu.at<uchar>(i, j) != 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) == 0) {
					cnt[tmplen]++;
					if (maxlen < tmplen) {
						maxlen = tmplen;
						maxi = tmpi;
						maxj = tmpj;   
						tmpi = -1;
						tmpj = -1;
						tmplen = 0;
					}
				}
			}
		}
	}
}

void countWhiteVerticalConsecutivePixels(int &width, int &height, int &maxi, int &maxj, int &maxlen, Mat &resultAfterPartialOtsu) {
	int tmplen;
	int tmpi = -1;
	int tmpj = -1;
	// 255 white

	for (int j = 0;j < width;j++) {
		for (int i = 0;i < height;i++) {
			if (i == 0) {
				if (tmpi != -1 && tmpj != -1) {
					cnt[tmplen]++;
					if (maxlen < tmplen) {
						maxlen = tmplen;
						maxi = tmpi;
						maxj = tmpj;
					}
					tmplen = 0;
				}
				if (resultAfterPartialOtsu.at<uchar>(i, j) != 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
			}
			else {
				// before black current white
				if (resultAfterPartialOtsu.at<uchar>(i, j) != 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) == 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
				// before white current white
				else if (resultAfterPartialOtsu.at<uchar>(i, j) != 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) != 0) {
					tmplen++;
				}
				// before white current black
				else if (resultAfterPartialOtsu.at<uchar>(i, j) == 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) != 0) {
					cnt[tmplen]++;
					if (maxlen < tmplen) {
						maxlen = tmplen;
						maxi = tmpi;
						maxj = tmpj;
						tmpi = -1;
						tmpj = -1;
						tmplen = 0;
					}
				}
			}
		}
	}
}

void findAndReverseWhiteVerticalConsecutivePixels(int &width, int &height, Mat &resultAfterPartialOtsu, int rangeStart, int rangeFinish) {
	int tmplen;
	int tmpi = -1;
	int tmpj = -1;
	// 255 white
	
	for (int j = 0;j < width;j++) {
		for (int i = 0;i < height;i++) {
			if (i == 0) {
				if (tmpi != -1 && tmpj != -1) {
					cnt[tmplen]++;
					if (tmplen >= rangeStart && tmplen <= rangeFinish) {
						resultAfterPartialOtsu = changeVerticalWhiteIntoBlack(resultAfterPartialOtsu, tmpi, tmpj, tmplen);
					}
					tmplen = 0;
				}
				if (resultAfterPartialOtsu.at<uchar>(i, j) != 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
			}
			else {
				// before black current white
				if (resultAfterPartialOtsu.at<uchar>(i, j) != 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) == 0) {
					tmpj = j;
					tmpi = i;
					tmplen = 1;
				}
				// before white current white
				else if (resultAfterPartialOtsu.at<uchar>(i, j) != 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) != 0) {
					tmplen++;
				}
				// before white current black
				else if (resultAfterPartialOtsu.at<uchar>(i, j) == 0 && resultAfterPartialOtsu.at<uchar>(i - 1, j) != 0) {
					cnt[tmplen]++;
					if (tmplen >= rangeStart && tmplen <= rangeFinish) {
						resultAfterPartialOtsu = changeVerticalWhiteIntoBlack(resultAfterPartialOtsu, tmpi, tmpj, tmplen);
					}
					tmpi = -1;
					tmpj = -1;
					tmplen = 0;
				}
			}
		}
	}
}

void printMaxIMaxJMaxLen(int maxi, int maxj, int maxlen) {
	cout << "i: " << maxi << endl;
	cout << "j: " << maxj << endl;
	cout << "len: " << maxlen << endl;
}

void prefsum() {
	dpSigmaXSquare[1] = 1;
	dpSigmaX[1] = 1;
	dpSigmaY[1] = cnt[1];
	dpSigmaXY[1] = cnt[1];

	for (int i = 2;i < 3005;i++) {
		dpSigmaXSquare[i] = dpSigmaXSquare[i - 1] + (i*i);
		dpSigmaX[i] = dpSigmaX[i - 1] + i;
		dpSigmaY[i] = dpSigmaY[i - 1] + cnt[i];
		dpSigmaXY[i] = dpSigmaXY[i - 1] + (i * cnt[i]);
	}
}

double solveGradient(int start, int window) {
	long long sigmaXSquare = dpSigmaXSquare[start + window - 1] - dpSigmaXSquare[start - 1];
	long long sigmaY = dpSigmaY[start + window - 1] - dpSigmaY[start - 1];
	long long sigmaX = dpSigmaX[start + window - 1] - dpSigmaX[start - 1];
	long long sigmaXY = dpSigmaXY[start + window - 1] - dpSigmaXY[start - 1];

	return (double)(window * sigmaXY - sigmaX * sigmaY) / (double)(window * sigmaXSquare - sigmaX * sigmaX);
}

void printData(int threshold, int window, string filename, string mode) {

	ofstream data(filename);

	if (data.is_open()) {

		if (mode == "GRADIENT") {
			for (int i = 1;i < 3500;i++) {
				if (cnt[i] > threshold && cnt[i + window - 1] > threshold) {
					data << i << ": " << cnt[i] << endl;
					data << "gradient: " << (double)solveGradient(i, window) << endl;
				}
			}
		}
		else if (mode == "DATA ONLY") {
			for (int i = 1;i < 3500;i++) {
				if (cnt[i] > threshold && cnt[i + window - 1] > threshold) {
					data << i << ": " << cnt[i] << " ";
				}
			}
		}
		

		data.close();
	}
	else {
		cout << "Unable to find file\n";
	}
}

void otsu(string input) {
	Mat im_gray;

	Mat im_rgb = imread(sample_pic_dir + "buku/" + input + ".jpg");
	cvtColor(im_rgb, im_gray, CV_RGB2GRAY);

	Mat img_bw = im_gray;
	
	int height = img_bw.rows;
	int width = img_bw.cols;

	Mat resultAfterPartialOtsu = im_gray;

	Size smallsize(width/16 , height/16);
	
	for (int i = 0; i+smallsize.height < height; i += smallsize.height) {
		for (int j = 0; j+smallsize.width < width; j += smallsize.width) {

			Mat outin = img_bw(Rect(j, i, width / 16, height / 16));
			Mat out;
			threshold(outin, out, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			out.copyTo(resultAfterPartialOtsu(Rect(j, i, out.cols, out.rows)));

			if (i + 2 * smallsize.height >= height || j + 2 * smallsize.width >= width) {
				int tmpposi = i + smallsize.height;
				int tmpposj = j + smallsize.width;
				outin = img_bw(Rect(tmpposj, tmpposi, width - tmpposj, height - tmpposi));
				threshold(outin, out, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

				out.copyTo(resultAfterPartialOtsu(Rect(tmpposj, tmpposi, out.cols, out.rows)));
			}

			String dir = output_pic_dir + "buku/sub/" + to_string(i) + "-" + to_string(j) + ".jpg";
			//imwrite(dir, out );
		}
	}

	String dir = output_pic_dir + "buku/" + input + "_partial_otsu16x16.jpg";
	//imwrite(dir , resultAfterPartialOtsu);
	
	
	int maxi;
	int maxj;
	int maxlen=0;
	
	findAndReverseWhiteVerticalConsecutivePixels(width, height, resultAfterPartialOtsu, 16, 18);

	//resultAfterPartialOtsu = changeVerticalBlackIntoWhite(resultAfterPartialOtsu, maxi, maxj, maxlen);
	
	dir = output_pic_dir + "buku/"+ input + "_partial_otsu16x16_edited15-18px.jpg";
	//imwrite(dir, resultAfterPartialOtsu);

	prefsum();

	printData(1, 4, "(Window 4) Data + Gradient  " + input + ".txt" , "GRADIENT");

	getchar();
}

int main()
{
	String filename="f5";
	otsu(filename);

	waitKey(0);
	
	destroyAllWindows();
	return 0;
}
