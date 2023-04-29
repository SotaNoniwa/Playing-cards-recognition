#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "playing_cards.hpp"

using namespace std;
using namespace cv;

int getNumberOnCard(Mat gray_card) {

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	params.filterByArea = true;
	params.minArea = 35;

	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	params.filterByConvexity = true;
	params.minConvexity = 0.85;

	params.filterByInertia = true;
	params.minInertiaRatio = 0.5;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	vector<KeyPoint> keypoints;
	detector->detect(gray_card, keypoints);

	Mat blob_img;
	drawKeypoints(gray_card, keypoints, blob_img, Scalar(0, 255, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//imshow("blobs", blob_img);

	// number of blobs = number of keypoints = number of suits on the card
	// then subtract by 2 (left upper corner and bottom right corner suits)
	return keypoints.size() - 2;
}

string getSuitOnCard(Mat& canvas, vector<vector<Point>>& contours, vector<vector<Point>>& conPoly) {

	// Create a vector of pairs where the first element is the contour index and the second element is the contour area
	vector<pair<int, double>> contour_areas(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		contour_areas[i] = make_pair(i, contourArea(contours[i]));
	}

	// ---------------- Detecting the contour of suit on the cards ---------------
	// Loop through the sorted vector of pairs and access the contours by their indices in descending order of area
	int max_contour_area = 0;
	int max_contour_area_index;

	for (const auto& contour_area : contour_areas) {
		int contour_index = contour_area.first;
		vector<Point> contour = contours[contour_index];

		// if you want to see all contours and their hierarchy, uncomment below
		//drawContours(canvas, contours, contour_index, Scalar(0, 255, 0), 1);
		/*putText(canvas, to_string(contour_index), contours[contour_index][0],
			FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 0, 0));*/

		// Skip index 0 and 1 (external contours of the two largest area (contours of background and card itself))
		if (1 < contour_index and max_contour_area < contour_area.second) {
			max_contour_area = contour_area.second;
			max_contour_area_index = contour_index;
		}
	}

	// ----------- Analysing the contour shape of the suit ---------------
	// Let's call this approPolyDP First filter
	float perimeter = arcLength(contours[max_contour_area_index], true); // return the length of closed contours perimeter
	approxPolyDP(contours[max_contour_area_index], conPoly[max_contour_area_index],
		0.02 * perimeter, true);

	// if you want to check the number of corners of suit contour by the First filter, uncomment below
	/*cout << "The length of each segment of contour is contour's perimeter * 0.02" << endl;
	cout << conPoly[max_contour_area_index].size() << endl;*/
	
	// Under the condition of First filter, the size of hearts and spades are detected around 9 or 10
	// I need to catch them and apply more accurate filter
	int min_size_heart_spade = 8;
	int max_size_heart_spade = 11;

	// the previous approxPolyDP (First filter) couldn't differenciate between hearts and spades,
	// thus we improve accuracy by make the length of each line (to draw contours) shorter
	// this is the Second filter (for hearts and spades)
	if (min_size_heart_spade <= conPoly[max_contour_area_index].size() and
		conPoly[max_contour_area_index].size() <= max_size_heart_spade)
	{
		approxPolyDP(contours[max_contour_area_index], conPoly[max_contour_area_index],
			0.001 * perimeter, true); // Notice here I replaced 0.02 to 0.001
		//cout << "The length of each segment of contour is contour's perimeter * 0.001" << endl;
	}

	int suit_coners = (int)conPoly[max_contour_area_index].size();

	// if you want to check the number of corners of suit contour after through First and Second filter
	//cout << suit_coners << endl;

	string suit;

	// Determine the suit based on the number of contours's coners
	if (suit_coners == 4) {
		suit = "Diamond";
	}
	else if (12 <= suit_coners and suit_coners <= 17) {
		suit = "Club";
	}
	else if (55 <= suit_coners and suit_coners <= 65) {
		suit = "Heart";
	}
	else { // else if (66 <= suit_coners)
		suit = "Spade";
	}

	drawContours(canvas, conPoly, max_contour_area_index, Scalar(0, 255, 0), 2);
	//drawContours(canvas, contours, max_contour_area_index, Scalar(0, 255, 0), 2);

	// just to display all contours and their areas in the hierarchy
	/*for (size_t i = 0; i < contour_areas.size(); i++) {
		int contour_index = contour_areas[i].first;
		double contour_area = contour_areas[i].second;
		cout << "Contour " << contour_index << " has an area of " << contour_area << endl;
	}*/

	// if you want to see the result image, uncomment below
	//imshow(suit, canvas);

	return suit;
}