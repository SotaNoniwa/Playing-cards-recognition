#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "playing_cards.hpp"

using namespace std;
using namespace cv;

int main() {
	Mat img, gray, mask;
	vector<vector<Point>> contours;
	vector<Vec4i> hier;

	for (int i = 1; i <= 10; i++) {
		String img_path = "cards/" + to_string(i) + ".png";
		img = imread(img_path, IMREAD_COLOR);

		if (img.empty()) {
			cout << "Could not read image " << img_path << endl;
			return -1;
		}

		// Pre-processing
		cvtColor(img, gray, COLOR_BGR2GRAY);
		threshold(gray, mask, 150, 255, THRESH_BINARY_INV);

		// TREE hierarchy is easy to identify the particular contour
		findContours(mask, contours, hier, RETR_TREE, CHAIN_APPROX_NONE);

		// Contour polygon is rough contour wichi consists of segment of lines
		vector<vector<Point>> conPoly(contours.size());

		int number = getNumberOnCard(gray);

		Mat canvas = img.clone();
		string suit = getSuitOnCard(canvas, contours, conPoly);

		imshow(suit + " " + to_string(number), img);
		cout << suit << " " << to_string(number) << endl;

		waitKey(0);
	}

	return 0;
}