#pragma once

#include "common.h"

int rectSize = 15;

struct mouseParam {
	int x, y, event, flags;
};

void mouseCallBack(int event, int x, int y, int flags, void* data) {
	mouseParam* ptr = (mouseParam*)data;
	ptr->x = x; ptr->y = y; ptr->event = event; ptr->flags = flags;
}


//クリックした場所にある2点をそれぞれキーの左上、右上として取得
void getKeyPosition(cv::Mat img, cv::Point& leftUp, cv::Point& rightDown, bool read = false) {
	mouseParam mouse;
	cv::Mat img_tmp;

	if (read) {

	}
	else {
		img.copyTo(img_tmp);

		cv::cvtColor(img_tmp, img_tmp, cv::COLOR_GRAY2RGB);

		int key = 0;
		cv::namedWindow("keyPosition");
		cv::setMouseCallback("keyPosition", mouseCallBack, &mouse);

		for (int i = 0; i < 2; i++) {
			mouse.event = NULL;
			while (mouse.event != cv::EVENT_LBUTTONDOWN) {
				cv::imshow("keyPosition", img_tmp);
				cv::waitKey(1);
			}
			mouse.event = NULL;
			int cx = mouse.x;
			int cy = mouse.y;
			if (i == 0) {
				leftUp.x = cx;
				leftUp.y = cy;
			}
			else {
				rightDown.x = cx;
				rightDown.y = cy;
			}
			cv::Rect rect;
			rect.x = std::max(0, cx - rectSize);
			rect.y = std::max(0, cy - rectSize);
			rect.width = std::min(rectSize+ img_tmp.cols - cx, std::min(rectSize + cx + 1,rectSize * 2 + 1));
			rect.height = std::min(rectSize + img_tmp.rows - cy, std::min(rectSize + cy + 1, rectSize * 2 + 1));

			cv::rectangle(img_tmp, rect, cv::Scalar(0,255,0));

			cv::circle(img_tmp, cv::Point(cx, cy), 2, cv::Scalar(0,255,0), 1);
			cv::imshow("keyPosition", img_tmp);
			cv::waitKey(33);
			printf("%d Detected (%d %d)\n", i, cx, cy);
		}
		cv::destroyWindow("keyPosition");
	}
}