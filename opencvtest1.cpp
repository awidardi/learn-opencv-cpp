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

int cnt[3005];

// vector of pair [column](starting row , length)
vector<pair<int, int> > startWhitePixel[3005];
vector<pair<int, int> > startWhitePixelAfter[3005];

// vector of pair [length](starting row , column)
vector<pair<int, int> > startCoordinateWhitePixel[3005];

long long dpSigmaXSquare[3005];
long long dpSigmaX[3005];
long long dpSigmaY[3005];
long long dpSigmaXY[3005];

int jedaBarisFrek[3005];

Mat changeVerticalWhiteIntoBlack(Mat image, int he, int wi, int len) {

	for (int i = 1;i <= len;i++) {
		image.at<uchar>(he + i - 1, wi) = 0;
	}
	return image;
}

Mat changeVerticalBlackIntoWhite(Mat image, int he, int wi, int len) {

	for (int i = 1;i <= len;i++) {
		image.at<uchar>(he + i - 1, wi) = 255;
	}
	return image;
}

void printMaxIMaxJMaxLen(int maxi, int maxj, int maxlen) {
	cout << "i: " << maxi << endl;
	cout << "j: " << maxj << endl;
	cout << "len: " << maxlen << endl;
}

Mat inputGrayscaleImage(string input, int &width, int &height) {

	Mat im_gray;

	Mat im_rgb = imread(sample_pic_dir + "buku/" + input + ".jpg");
	cvtColor(im_rgb, im_gray, CV_RGB2GRAY);

	height = im_gray.rows;
	width = im_gray.cols;

	return im_gray;
}

Mat clahe(Mat input, int limit) {

	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(limit);

	Mat output;

	clahe->apply(input, output);

	return output;

}

Mat otsu(Mat imageAfterClahe, int width, int height) {
	String dir;

	Mat img_bw = imageAfterClahe;

	Mat resultAfterPartialOtsu = imageAfterClahe;

	Size smallsize(width / 16, height / 16);

	for (int i = 0; i + smallsize.height < height; i += smallsize.height) {
		for (int j = 0; j + smallsize.width < width; j += smallsize.width) {

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
		}
	}

	return resultAfterPartialOtsu;

	//dir = output_pic_dir + "buku/"+ input + "_partial_otsu16x16_edited9-13px.jpg";
	//dir = output_pic_dir + "buku/"+ input + "_partial+clahe2_otsu16x16_edited10-19px.jpg";
	//imwrite(dir, resultAfterPartialOtsu);
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
			for (int i = 1;i < 3005;i++) {
				if (cnt[i] > threshold && cnt[i + window - 1] > threshold) {
					data << i << ": " << cnt[i] << endl;
					data << "gradient: " << (double)solveGradient(i, window) << endl;
				}
			}
		}
		else if (mode == "DATA ONLY") {
			for (int i = 1;i < 3005;i++) {
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

long long findStartMaxGradient(int threshold, int window) {

	int indexMax = 1;
	long long valueMax = -1;

	for (int i = 1;i < 3005;i++) {
		if (cnt[i] > threshold && cnt[i + window - 1] > threshold) {
			long long tmp = solveGradient(i, window);
			if (tmp > valueMax) {
				indexMax = i;
				valueMax = tmp;
			}
		}
	}
	return indexMax;

}

int findNormalPoint(int start , int window) {
	long long sum = 0;
	long long divisor = 0;
	for (int i = start;i <= start + window - 1;i++) {
		sum += (i * cnt[i]);
		divisor += cnt[i];
	}

	return (int)(sum / divisor);
}

Mat findAndReverseWhiteVerticalConsecutivePixels(Mat image, int rangeStart, int rangeFinish) {
	
	for (int i = rangeStart;i <= rangeFinish;i++) {
		for (int j = 0;j < startCoordinateWhitePixel[i].size();j++) {
			image = changeVerticalWhiteIntoBlack(image, startCoordinateWhitePixel[i][j].first, startCoordinateWhitePixel[i][j].second , i);
		}
	}

	return image;
}

void countJedaBarisAtPartition(int &width, int &height, Mat &resultAfterPartialOtsu, int rangeStart, int rangeFinish) {
	
	Size smallsize(width / 16, height / 16);

	for (int j = 0;j < width;j+=smallsize.width) {
		


	}
}

Mat fixVectorImage(int width, int height, int normalPoint ,int startThreshold, int windowThreshold) {

	for (int j = 0;j < width;j++) {
		int add = 0;

		int sz = startWhitePixel[j].size();

		for (int i = 0;i < sz;i++) {
			
			int fi = startWhitePixel[j][i].first;
			int se = startWhitePixel[j][i].second;
			int len;

			if (i!=0) {
				if ((se >= startThreshold && se <= startThreshold + windowThreshold - 1)) {
					add = add + (normalPoint - se);
					len = min(height - (fi + add) , normalPoint);
				}
				else {
					len = min(height - (fi + add), se);
				}

				startWhitePixelAfter[j].push_back(make_pair(fi + add, len));
			}
		}
	}

	Mat newImage(height, width, CV_8UC1, Scalar(0, 0, 0));

	for (int j = 0;j < width;j++) {
		for (int i = 0;i < startWhitePixelAfter[j].size();i++) {
			newImage = changeVerticalBlackIntoWhite(newImage, startWhitePixelAfter[j][i].first, j, startWhitePixelAfter[j][i].second);
		}
	}

	return newImage;
}

void solverWhitePixel(Mat resultAfterPartialOtsu, int width, int height) {

	int tmplen;
	int tmpi = -1;
	int tmpj = -1;

	for (int j = 0;j < width;j++) {
		for (int i = 0;i < height;i++) {
			if (i == 0) {
				if (tmpi != -1 && tmpj != -1) {
					cnt[tmplen]++;
					startWhitePixel[tmpj].push_back( make_pair(tmpi , tmplen) );
					startCoordinateWhitePixel[tmplen].push_back(make_pair(tmpi, tmpj));
					jedaBarisFrek[tmpj]++;
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
					startWhitePixel[tmpj].push_back( make_pair(tmpi , tmplen) );
					startCoordinateWhitePixel[tmplen].push_back(make_pair(tmpi, tmpj));
					jedaBarisFrek[tmpj]++;
					tmpi = -1;
					tmpj = -1;
					tmplen = 0;
				}
			}
		}
	}
}

void imadjust(const Mat1b& src, Mat1b& dst, int tol = 1, Vec2i in = Vec2i(0, 255), Vec2i out = Vec2i(0, 255))
{
	// src : input CV_8UC1 image
	// dst : output CV_8UC1 imge
	// tol : tolerance, from 0 to 100.
	// in  : src image bounds
	// out : dst image buonds

	dst = src.clone();

	tol = max(0, min(100, tol));

	if (tol > 0)
	{
		// Compute in and out limits

		// Histogram
		vector<int> hist(256, 0);
		for (int r = 0; r < src.rows; ++r) {
			for (int c = 0; c < src.cols; ++c) {
				hist[src(r, c)]++;
			}
		}

		// Cumulative histogram
		vector<int> cum = hist;
		for (int i = 1; i < hist.size(); ++i) {
			cum[i] = cum[i - 1] + hist[i];
		}

		// Compute bounds
		int total = src.rows * src.cols;
		int low_bound = total * tol /100;
		int upp_bound = total * (100 - tol) / 100;
		in[0] = distance(cum.begin(), lower_bound(cum.begin(), cum.end(), low_bound));
		in[1] = distance(cum.begin(), lower_bound(cum.begin(), cum.end(), upp_bound));

	}

	// Stretching
	float scale = float(out[1] - out[0]) / float(in[1] - in[0]);
	for (int r = 0; r < dst.rows; ++r)
	{
		for (int c = 0; c < dst.cols; ++c)
		{
			int vs = max(src(r, c) - in[0], 0);
			int vd = min(int(vs * scale + 0.5f) + out[0], out[1]);
			dst(r, c) = saturate_cast<uchar>(vd);
		}
	}
}

int main()
{
	double startAll = clock(), start, stop;
	int height, width;
	String filename="g1";
	String dir;

	// TODO : INPUT IMAGE AND CONVERT TO GRAYSCALE

	start = clock();
	Mat imageInput = inputGrayscaleImage(filename, width, height);
	cout << "INPUT Time Execution : " << (double) (clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : CONTRAST ENHANCEMENT WITH CLAHE

	start = clock();
	Mat imageAfterClahe = clahe(imageInput, 2);
	cout << "CONTRAST ENHANCEMENT Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : BINARIZATION WITH OTSU METHOD
	
	start = clock();
	Mat imageAfterOtsu = otsu(imageAfterClahe, width, height);
	dir = output_pic_dir + "buku/otsu/" + filename + ".jpg";
	imwrite(dir , imageAfterOtsu);
	cout << "OTSU Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : PROCESSING WHITE PIXEL SOLVER

	start = clock();
	solverWhitePixel(imageAfterOtsu, width, height);
	cout << "MENCARI JEDA BARIS Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : PRE - COMPUTE PREFSUM DP

	start = clock();
	prefsum();
	cout << "PENYIMPANAN DATA 1 Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : PRINT GRADIENT OR SAVE THE DATA AT FILE

	// dir = output_pic_dir + "buku/data/(Window 10) Data + Gradient  " + filename + ".txt";
	// printData(1, 5, dir , "GRADIENT");

	// TODO : SOLVE REGRESSION AND FIND MAX GRAD

	// THRESHOLD = 1 , WINDOW = 10
	start = clock();
	int pointStartMaxGrad = findStartMaxGradient(1, 10);
	cout << "MENCARI MAX GRADIENT DENGAN REGRESI Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;

	// TODO : FIND NORMAL POINT

	start = clock();
	int normalPoint = findNormalPoint(pointStartMaxGrad, 10);
	cout << "MENCARI JEDA BARIS DEFAULT Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
	
	// TODO : REVERSE WHITE PIXEL INTO BLACK AND OUTPUT

	// Mat imageValidation = findAndReverseWhiteVerticalConsecutivePixels(imageAfterOtsu, pointStartMaxGrad, pointStartMaxGrad + 9);
	// dir = output_pic_dir + "buku/hasil/" + filename + ".jpg";
	// imwrite(dir, imageValidation);

	// TODO : FIX IMAGE

	start = clock();
	Mat imageResult = fixVectorImage(width, height, normalPoint, pointStartMaxGrad, 10);
	dir = output_pic_dir + "buku/result/" + filename + ".jpg";
	imwrite(dir, imageResult);
	cout << "PENYISIPAN DAN PEMBETULAN GAMBAR Time Execution : " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;


	cout << "TOTAL Time Execution : " << (double)(clock() - startAll) / CLOCKS_PER_SEC << endl;
	getchar();

	waitKey(0);
	
	destroyAllWindows();
}
