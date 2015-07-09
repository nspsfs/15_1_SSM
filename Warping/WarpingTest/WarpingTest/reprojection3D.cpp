#if 0

#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]){

	cv::initModule_nonfree();

	// Read the images
	//Mat imgLeft = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	//Mat imgRight = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgLeft = imread("image/b.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgRight = imread("image/a.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// check
	if (!imgLeft.data || !imgRight.data)
		return 0;

	// 1] find pair keypoints on both images (SURF, SIFT):::::::::::::::::::::::::::::

	// vector of keypoints
	std::vector<cv::KeyPoint> keypointsLeft;
	std::vector<cv::KeyPoint> keypointsRight;


	// Construct the SURF feature detector object
	cv::SiftFeatureDetector sift(
		0.01, // feature threshold
		10); // threshold to reduce
	// sensitivity to lines
	// Detect the SURF features

	// Detection of the SIFT features
	sift.detect(imgLeft, keypointsLeft);
	sift.detect(imgRight, keypointsRight);

	std::cout << "Number of SURF points (1): " << keypointsLeft.size() << std::endl;
	std::cout << "Number of SURF points (2): " << keypointsRight.size() << std::endl;

	// 2] compute descriptors of these keypoints (SURF,SIFT) ::::::::::::::::::::::::::

	// Construction of the SURF descriptor extractor
	cv::SurfDescriptorExtractor surfDesc;

	// Extraction of the SURF descriptors
	cv::Mat descriptorsLeft, descriptorsRight;
	surfDesc.compute(imgLeft, keypointsLeft, descriptorsLeft);
	surfDesc.compute(imgRight, keypointsRight, descriptorsRight);

	std::cout << "descriptor matrix size: " << descriptorsLeft.rows << " by " << descriptorsLeft.cols << std::endl;

	// 3] matching keypoints from image right and image left according to their descriptors (BruteForce, Flann based approaches)

	// Construction of the matcher
	cv::BruteForceMatcher<cv::L2<float> > matcher;

	// Match the two image descriptors
	std::vector<cv::DMatch> matches;
	matcher.match(descriptorsLeft, descriptorsRight, matches);

	std::cout << "Number of matched points: " << matches.size() << std::endl;


	// 4] find the fundamental mat ::::::::::::::::::::::::::::::::::::::::::::::::::::

	// Convert 1 vector of keypoints into
	// 2 vectors of Point2f for compute F matrix
	// with cv::findFundamentalMat() function
	std::vector<int> pointIndexesLeft;
	std::vector<int> pointIndexesRight;
	for (std::vector<cv::DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it) {

		// Get the indexes of the selected matched keypoints
		pointIndexesLeft.push_back(it->queryIdx);
		pointIndexesRight.push_back(it->trainIdx);
	}

	// Convert keypoints into Point2f
	std::vector<cv::Point2f> selPointsLeft, selPointsRight;
	cv::KeyPoint::convert(keypointsLeft, selPointsLeft, pointIndexesLeft);
	cv::KeyPoint::convert(keypointsRight, selPointsRight, pointIndexesRight);

	// //check by drawing the points
	//std::vector<cv::Point2f>::const_iterator it= selPointsLeft.begin();
	//while (it!=selPointsLeft.end()) {

	//// draw a circle at each corner location
	//cv::circle(imgLeft,*it,3,cv::Scalar(255,255,255),2);
	//++it;
	//}

	//it= selPointsRight.begin();
	//while (it!=selPointsRight.end()) {

	//// draw a circle at each corner location
	//cv::circle(imgRight,*it,3,cv::Scalar(255,255,255),2);
	//++it;
	//} 

	// Compute F matrix from n>=8 matches
	cv::Mat fundemental = cv::findFundamentalMat(
		cv::Mat(selPointsLeft), // points in first image
		cv::Mat(selPointsRight), // points in second image
		CV_FM_RANSAC);       // 8-point method

	std::cout << "F-Matrix size= " << fundemental.rows << "," << fundemental.cols << std::endl;

	/* // draw the left points corresponding epipolar lines in right image
	std::vector<cv::Vec3f> linesLeft;
	cv::computeCorrespondEpilines(
	cv::Mat(selPointsLeft), // image points
	1,                      // in image 1 (can also be 2)
	fundemental,            // F matrix
	linesLeft);             // vector of epipolar lines

	// for all epipolar lines
	for (vector<cv::Vec3f>::const_iterator it= linesLeft.begin(); it!=linesLeft.end(); ++it) {

	// draw the epipolar line between first and last column
	cv::line(imgRight,cv::Point(0,-(*it)[2]/(*it)[1]),cv::Point(imgRight.cols,-((*it)[2]+(*it)[0]*imgRight.cols)/(*it)[1]),cv::Scalar(255,255,255));
	}

	// draw the left points corresponding epipolar lines in left image
	std::vector<cv::Vec3f> linesRight;
	cv::computeCorrespondEpilines(cv::Mat(selPointsRight),2,fundemental,linesRight);
	for (vector<cv::Vec3f>::const_iterator it= linesRight.begin(); it!=linesRight.end(); ++it) {

	// draw the epipolar line between first and last column
	cv::line(imgLeft,cv::Point(0,-(*it)[2]/(*it)[1]), cv::Point(imgLeft.cols,-((*it)[2]+(*it)[0]*imgLeft.cols)/(*it)[1]), cv::Scalar(255,255,255));
	}

	// Display the images with points and epipolar lines
	cv::namedWindow("Right Image Epilines");
	cv::imshow("Right Image Epilines",imgRight);
	cv::namedWindow("Left Image Epilines");
	cv::imshow("Left Image Epilines",imgLeft);
	*/

	// 5] stereoRectifyUncalibrated()::::::::::::::::::::::::::::::::::::::::::::::::::

	//H1, H2 – The output rectification homography matrices for the first and for the second images.
	cv::Mat H1(4, 4, imgRight.type());
	cv::Mat H2(4, 4, imgRight.type());
	cv::stereoRectifyUncalibrated(selPointsRight, selPointsLeft, fundemental, imgRight.size(), H1, H2);


	// create the image in which we will save our disparities
	Mat imgDisparity16S = Mat(imgLeft.rows, imgLeft.cols, CV_16S);
	Mat imgDisparity8U = Mat(imgLeft.rows, imgLeft.cols, CV_8UC1);

	// Call the constructor for StereoBM
	int ndisparities = 16 * 5;      // < Range of disparity >
	int SADWindowSize = 5;        // < Size of the block window > Must be odd. Is the 
	// size of averaging window used to match pixel  
	// blocks(larger values mean better robustness to
	// noise, but yield blurry disparity maps)

	StereoBM sbm(StereoBM::BASIC_PRESET,
		ndisparities,
		SADWindowSize);

	// Calculate the disparity image
	sbm(imgLeft, imgRight, imgDisparity16S, CV_16S);

	// Check its extreme values
	double minVal; double maxVal;

	minMaxLoc(imgDisparity16S, &minVal, &maxVal);

	printf("Min disp: %f Max value: %f \n", minVal, maxVal);

	// Display it as a CV_8UC1 image
	imgDisparity16S.convertTo(imgDisparity8U, CV_8UC1, 255 / (maxVal - minVal));

	namedWindow("windowDisparity", CV_WINDOW_NORMAL);
	imshow("windowDisparity", imgDisparity8U);


	// 6] reprojectImageTo3D() :::::::::::::::::::::::::::::::::::::::::::::::::::::
	int q[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 152 }, { 0, 0, 0, 0 } };
	Mat Q(4, 4, CV_8UC1,q);
	
	Mat xyz;
	cv::reprojectImageTo3D(imgDisparity8U, xyz, Q, true);
	imshow("test", xyz);
		

	//How can I get the Q matrix? Is possibile to obtain the Q matrix with 
	//F, H1 and H2 or in another way?
	//Is there another way for obtain the xyz coordinates?

	cv::waitKey();
	return 0;
}

#endif
 