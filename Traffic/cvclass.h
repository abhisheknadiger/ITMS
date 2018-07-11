#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<fstream>
#include"algo.h"
using namespace cv;
class Blob {
public:
	std::vector<Point> Contour;
	Rect boundrect;

	std::vector<Point> center;

	double diagonal;
	double ratio;
	bool match;
	bool track;
	int frameno;
	Point next_pos;
	Blob(std::vector<Point> _contour);
	void Predict(void);
};
Blob::Blob(std::vector<Point> _contour) {

	Contour = _contour;

	boundrect = boundingRect(Contour);

	Point currentCenter;

	currentCenter.x = (boundrect.x + boundrect.x + boundrect.width) / 2;
	currentCenter.y = (boundrect.y + boundrect.y + boundrect.height) / 2;
	center.push_back(currentCenter);
	diagonal = sqrt(pow(boundrect.width, 2) + pow(boundrect.height, 2));
	ratio = (float)boundrect.width / (float)boundrect.height;

	track = true;
	match = true;

	frameno = 0;
}
void Blob::Predict(void) {

	int numPositions = (int)center.size();

	if (numPositions == 1) {

		next_pos.x = center.back().x;
		next_pos.y = center.back().y;

	}
	else if (numPositions == 2) {

		int deltaX = center[1].x - center[0].x;
		int deltaY = center[1].y - center[0].y;

		next_pos.x = center.back().x + deltaX;
		next_pos.y = center.back().y + deltaY;

	}
	else if (numPositions == 3) {

		int Xchange = ((center[2].x - center[1].x) * 2) +
			((center[1].x - center[0].x) * 1);

		int deltaX = (int)std::round((float)Xchange / 3.0);

		int Ychange = ((center[2].y - center[1].y) * 2) +
			((center[1].y - center[0].y) * 1);

		int deltaY = (int)std::round((float)Ychange / 3.0);

		next_pos.x = center.back().x + deltaX;
		next_pos.y = center.back().y + deltaY;

	}
	else if (numPositions == 4) {

		int Xchange = ((center[3].x - center[2].x) * 3) +
			((center[2].x - center[1].x) * 2) +
			((center[1].x - center[0].x) * 1);

		int deltaX = (int)std::round((float)Xchange / 6.0);

		int Ychange = ((center[3].y - center[2].y) * 3) +
			((center[2].y - center[1].y) * 2) +
			((center[1].y - center[0].y) * 1);

		int deltaY = (int)std::round((float)Ychange / 6.0);

		next_pos.x = center.back().x + deltaX;
		next_pos.y = center.back().y + deltaY;

	}
	else if (numPositions >= 5) {

		int Xchange = ((center[numPositions - 1].x - center[numPositions - 2].x) * 4) +
			((center[numPositions - 2].x - center[numPositions - 3].x) * 3) +
			((center[numPositions - 3].x - center[numPositions - 4].x) * 2) +
			((center[numPositions - 4].x - center[numPositions - 5].x) * 1);

		int deltaX = (int)std::round((float)Xchange / 10.0);

		int Ychange = ((center[numPositions - 1].y - center[numPositions - 2].y) * 4) +
			((center[numPositions - 2].y - center[numPositions - 3].y) * 3) +
			((center[numPositions - 3].y - center[numPositions - 4].y) * 2) +
			((center[numPositions - 4].y - center[numPositions - 5].y) * 1);

		int deltaY = (int)std::round((float)Ychange / 10.0);

		next_pos.x = center.back().x + deltaX;
		next_pos.y = center.back().y + deltaY;

	}
	else;
}
