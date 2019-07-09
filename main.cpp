// xiApiPlusOcvExample.cpp : program opens first camera, captures and displays 40 images


#include"common.h"

using namespace cv;
using namespace std;

#pragma region Parameters

int cam_height=400;
int cam_width = 500;

cv::Point keyLeftUpper = cv::Point(100, 100);
cv::Point keyRightDown = cv::Point(340, 310);


#pragma endregion



int main(int argc, char* argv[])
{
#pragma region HSC_OPEN
	try
	{
		// Sample for XIMEA OpenCV
		xiAPIplusCameraOcv cam;

		// Retrieving a handle to the camera device
		printf("Opening first camera...\n");
		cam.OpenFirst();

		cam.SetHeight(cam_height);
		cam.SetWidth(cam_width);
		
		// Set exposure
		cam.SetExposureTime(1000); //10000 us = 10 ms
		// Note: The default parameters of each camera might be different in different API versions
		
		printf("Starting acquisition...\n");
		cam.StartAcquisition();
		
		printf("First pixel value \n");
		XI_IMG_FORMAT format = cam.GetImageDataFormat();
#pragma endregion[


#pragma region prepare
		Mat cv_mat_image = cam.GetNextImageOcvMat();
		if (format == XI_RAW16 || format == XI_MONO16)
			normalize(cv_mat_image, cv_mat_image, 0, 65536, NORM_MINMAX, -1, Mat());// 0 - 65536, 16 bit unsigned integer range

		getKeyPosition(cv_mat_image, keyLeftUpper, keyRightDown, false);
#pragma endregion
		
#pragma region Main_thread
		int key = 0;
		while ((key=cv::waitKey(1))!='q') {
			Mat cv_mat_image = cam.GetNextImageOcvMat();
			if (format == XI_RAW16 || format == XI_MONO16)
				normalize(cv_mat_image, cv_mat_image, 0, 65536, NORM_MINMAX, -1, Mat());// 0 - 65536, 16 bit unsigned integer range

			if (key == 'h') {
				getKeyPosition(cv_mat_image, keyLeftUpper, keyRightDown, false);
			}

			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			
			LARGE_INTEGER start, end;
			QueryPerformanceCounter(&start);


			Mat dst;
			cv::threshold(cv_mat_image, dst, 0, 30, cv::THRESH_BINARY | cv::THRESH_OTSU);
			cv::threshold(cv_mat_image, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

			vector<vector<cv::Point>> contours;

			cv::findContours(dst, contours, cv::RETR_LIST ,cv::CHAIN_APPROX_NONE);

			double marea = 0.0;
			int max_area_contour = -1;
			for (int i = 0; i < contours.size(); i++) {
				double area = contourArea(contours.at(i));
				if (marea < area) {
					marea = area;
					max_area_contour = i;
				}
			}
			int y = dst.rows;
			int x=0;

			
			for (int j = 0; j < contours[max_area_contour].size(); j++) {
				cv::Point p = contours[max_area_contour][j];
				if (p.y < y) {
					y = p.y;
					x = p.x;
				}
			}
			
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2RGB);
			cv::rectangle(dst, keyLeftUpper, keyRightDown, cv::Scalar(0, 0, 200), 3, 4);
			cv::circle(dst, cv::Point(x, y), 10, cv::Scalar(0, 200, 0), 4, 8);

			

			QueryPerformanceCounter(&end);
			
			cv::imshow("Image from camera", dst);

			double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;

			//cout << time << endl;

			
		}
#pragma endregion

#pragma region CloseCamera
		
		cam.StopAcquisition();
		cam.Close();
		printf("Done\n");
		
		cvWaitKey(500);
	}
#pragma endregion
	catch(xiAPIplus_Exception& exp)
	{
		printf("Error:\n");
		exp.PrintError();
#ifdef WIN32
		Sleep(2000);
#endif
		cvWaitKey(2000);
		return -1;
	}
	return 0;
}

