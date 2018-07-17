#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<fstream>
#include"algo.h"
#include"cvclass.h"
using namespace cv;

const Scalar SCALAR_GREEN = Scalar(0.0, 255.0, 0.0);
const Scalar SCALAR_RED = Scalar(0.0, 0.0, 255.0);
const Scalar SCALAR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_YELLOW = Scalar(0.0, 255.0, 255.0);
void matchBlob(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distances(Point point1, Point point2);
void drawAndShowContours(Size imageSize, std::vector<std::vector<Point> > contours, std::string strImageName);
void drawAndShowContours(Size imageSize, std::vector<Blob> blobs, std::string strImageName);
bool Linecrossed(Mat &i, std::vector<Blob> &blobs, int &HorizontalLIne, int carCount[]);
bool Linecrossed1(std::vector<Blob> &blobs, int &HorizontalLIne, int& carCount);
void drawCarCountOnImage(int &carCount, Mat &imgFrame2Copy, int n);
void drawCarCountOnImage1(int &carCount, Mat &imgFrame2Copy);
int main()
{
	VideoCapture capVideo;

	Mat imgFrame1;
	Mat imgFrame2;

	std::ofstream myfile;
	myfile.open("Output.txt", std::ios::app);

	std::vector<Blob> blobs;

	Point crossingLine[2], crossLine[2];

	int carCount[5] = { 0 ,0, 0 ,0, 0 }, carCount1 = 0;

	capVideo.open("1.mp4");

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);

	int HorizontalLIne = (int)std::round((double)imgFrame1.rows * 0.85);

	int HorizontalLIne1 = (int)std::round((double)imgFrame1.rows * 0.15);

	crossingLine[0].x = imgFrame1.cols*.05;
	crossingLine[0].y = HorizontalLIne;

	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = HorizontalLIne;

	crossLine[0].x = 0;
	crossLine[0].y = HorizontalLIne1;

	crossLine[1].x = imgFrame1.cols *.3;
	crossLine[1].y = HorizontalLIne1;

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	int frameCount = 2;
	int i = 0;
	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		std::vector<Blob> currentFrameBlobs;

		Mat imgFrame1Copy = imgFrame1.clone();
		Mat imgFrame2Copy = imgFrame2.clone();

		Mat imgDifference;
		Mat imgThresh;

		cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		GaussianBlur(imgFrame1Copy, imgFrame1Copy, Size(5, 5), 0);
		GaussianBlur(imgFrame2Copy, imgFrame2Copy, Size(5, 5), 0);

		absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		imshow("imgThresh", imgThresh);
		Mat structuringElement5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
		for (unsigned int i = 0; i < 2; i++) {
			dilate(imgThresh, imgThresh, structuringElement5x5);
			dilate(imgThresh, imgThresh, structuringElement5x5);
			erode(imgThresh, imgThresh, structuringElement5x5);
		}

		Mat imgThreshCopy = imgThresh.clone();

		std::vector<std::vector<Point> > contours;

		findContours(imgThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		drawAndShowContours(imgThresh.size(), contours, "imgContours");

		std::vector<std::vector<Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);
		}

		drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.boundrect.area() > 400 &&
				possibleBlob.ratio > 0.2 &&
				possibleBlob.ratio < 4.0 &&
				possibleBlob.boundrect.width > 30 &&
				possibleBlob.boundrect.height > 30 &&
				possibleBlob.diagonal > 60.0 &&
				(contourArea(possibleBlob.Contour) / (double)possibleBlob.boundrect.area()) > 0.50) {
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		else {
			matchBlob(blobs, currentFrameBlobs);
		}

		drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		imgFrame2Copy = imgFrame2.clone();

		bool blnAtLeastOneBlobCrossedTheLine = Linecrossed(imgFrame2Copy, blobs, HorizontalLIne, carCount);
		bool blnAtLeastOneBlobCrossedTheLine1 = Linecrossed1(blobs, HorizontalLIne1, carCount1);
		if (blnAtLeastOneBlobCrossedTheLine == true) {
			line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
		}
		else {
			line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
		}
		if (blnAtLeastOneBlobCrossedTheLine1 == true) {
			line(imgFrame2Copy, crossLine[0], crossLine[1], SCALAR_GREEN, 2);
		}
		else {
			line(imgFrame2Copy, crossLine[0], crossLine[1], SCALAR_RED, 2);
		}
		for (int l = 0; l<5; l++)
			drawCarCountOnImage(carCount[l], imgFrame2Copy, l + 2);
		drawCarCountOnImage1(carCount1, imgFrame2Copy);

		imshow("imgFrame2Copy", imgFrame2Copy);

		currentFrameBlobs.clear();

		imgFrame1 = imgFrame2.clone();

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame2);
		}
		else {
			std::cout << "end of video\n";
			break;
		}
		i++;
		myfile << "Frame=" << i + 1 << "\n";
		myfile << carCount;
		myfile << "\n";
		blnFirstFrame = false;
		frameCount++;
		chCheckForEscKey = waitKey(1);

	}
	destroyAllWindows();
	myfile.close();
	if (chCheckForEscKey != 27)
		waitKey(0);
	//	second_main(carCount, carCount1);
	//	std::cin >> carCount;
}
void matchBlob(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.match = false;

		existingBlob.Predict();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			if (existingBlobs[i].track == true) {

				double dblDistance = distances(currentFrameBlob.center.back(), existingBlobs[i].next_pos);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.diagonal * 0.5) {
			addBlob(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.match == false) {
			existingBlob.frameno++;
		}

		if (existingBlob.frameno >= 5) {
			existingBlob.track = false;
		}

	}

}
void addBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].Contour = currentFrameBlob.Contour;
	existingBlobs[intIndex].boundrect = currentFrameBlob.boundrect;

	existingBlobs[intIndex].center.push_back(currentFrameBlob.center.back());

	existingBlobs[intIndex].diagonal = currentFrameBlob.diagonal;
	existingBlobs[intIndex].ratio = currentFrameBlob.ratio;

	existingBlobs[intIndex].track = true;
	existingBlobs[intIndex].match = true;
}
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.match = true;

	existingBlobs.push_back(currentFrameBlob);
}
double distances(Point point1, Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}
void drawAndShowContours(Size imageSize, std::vector<std::vector<Point> > contours, std::string strImageName) {
	Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	drawContours(image, contours, -1, SCALAR_WHITE, -1);

	imshow(strImageName, image);
}
void drawAndShowContours(Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<Point> > contours;

	for (auto &blob : blobs) {
		if (blob.track == true) {
			contours.push_back(blob.Contour);
		}
	}

	drawContours(image, contours, -1, SCALAR_WHITE, -1);

	imshow(strImageName, image);
}
bool checkline(Point p, Point q, Point a)
{
	if (a.x > p.x &&a.x < q.x)
		return true;
	return false;
}
bool Linecrossed(Mat& i, std::vector<Blob> &blobs, int &HorizontalLIne, int  carCount[]) {
	bool blnAtLeastOneBlobCrossedTheLine = false;
	Point lane[6];
	lane[0].x = 0;
	lane[0].y = HorizontalLIne;
	for (int i = 0; i < 6; i++)
	{
		lane[i].y = HorizontalLIne;
	}
	lane[0].x = 0;
	lane[1].x = 300;
	lane[2].x = 580;
	lane[3].x = 850;
	lane[4].x = 1110;
	lane[5].x = 1450;
	//line(i, lane[0], lane[1], SCALAR_RED, 2);
	//line(i, lane[4], lane[5], SCALAR_GREEN, 2);
	//int threshold = 100;
	for (auto blob : blobs) {

		if (blob.track == true && blob.center.size() >= 2) {
			int prevFrameIndex = (int)blob.center.size() - 2;
			int currFrameIndex = (int)blob.center.size() - 1;

			if (blob.center[prevFrameIndex].y > HorizontalLIne && blob.center[currFrameIndex].y < HorizontalLIne) {
				for (int i = 0; i<5; i++)
					if (checkline(lane[i], lane[i + 1], blob.center[currFrameIndex]))
						carCount[i]++;
				blnAtLeastOneBlobCrossedTheLine = true;
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}
void drawBlobInfoOnImage(std::vector<Blob> &blobs, Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].track == true) {
			rectangle(imgFrame2Copy, blobs[i].boundrect, SCALAR_RED, 2);

			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].diagonal / 60.0;
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			putText(imgFrame2Copy, std::to_string(i), blobs[i].center.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}
void drawCarCountOnImage(int &carCount, Mat &imgFrame2Copy, int n) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);
	Size textSize = getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);
	Point ptTextBottomLeftPosition;
	ptTextBottomLeftPosition.x = (int)((double)textSize.width * 1.25)*(n + 5);
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);
	putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
}
void drawCarCountOnImage1(int &carCount, Mat &imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);
	Size textSize = getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);
	Point ptTextBottomLeftPosition;
	ptTextBottomLeftPosition.x = 1 + (int)((double)textSize.width * 1.25);
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);
	putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
}
bool Linecrossed1(std::vector<Blob> &blobs, int &HorizontalLIne, int & carCount) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.track == true && blob.center.size() >= 2) {
			int prevFrameIndex = (int)blob.center.size() - 2;
			int currFrameIndex = (int)blob.center.size() - 1;

			if (blob.center[prevFrameIndex].y < HorizontalLIne && blob.center[currFrameIndex].y > HorizontalLIne) {
				if (blob.boundrect.area()>1200)
					carCount++;
				blnAtLeastOneBlobCrossedTheLine = true;
			}
		}
	}
	return blnAtLeastOneBlobCrossedTheLine;
}
