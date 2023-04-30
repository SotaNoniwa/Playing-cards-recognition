#pragma once

/*Return the number written on the playing card image.
The function takes gray scale image of playing card. Then calculate its number based on the number of suits (blobs).*/
int getNumberOnCard(cv::Mat gray_card);

/*Return the suit type shown on the playing card image.
The function takes two arguments:
canvas: you will draw contours on this canvas
contours: the original contours you want to approximate with polygonal approximation method
*/
std::string getSuitOnCard(cv::Mat& canvas, std::vector<std::vector<cv::Point>> contours);
