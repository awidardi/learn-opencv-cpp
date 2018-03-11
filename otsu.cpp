#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char **argv)
{
	Mat im_gray = imread("nasgor.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	Mat im_rgb = imread("nasgor.jpg");
	cvtColor(im_rgb, im_gray, CV_RGB2GRAY);

	Mat img_bw;

	threshold(im_gray, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	imwrite("D:/img_bw3.jpg", img_bw);

	waitkey(0);

	return 0;
}