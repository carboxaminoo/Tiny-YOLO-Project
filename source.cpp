#include <iostream>
#include "cv.hpp"
#include <opencv2/dnn.hpp>
#include<fstream>

using namespace std;
using namespace cv;
using namespace dnn;


Mat SENSOR(Mat frame,bool car, bool people, bool front, bool lane);

int main() {
	VideoCapture cap;

	Mat canny_left, canny_right, canny_middle;
	vector<Vec2f> lines1, lines2, lines3,lines4;

	Scalar object_roi_green(0, 255, 0);
	Scalar object_roi_red(0, 0, 255);

	//Yolo
	String modelConfiguration = "yolov2-tiny.cfg";
	String modelBinary = "yolov2-tiny.weights";
	Net net = readNetFromDarknet(modelConfiguration, modelBinary);

	vector<String> classNamesVec;
	ifstream classNamesFile("coco.names");

	if (classNamesFile.is_open()) {
		string className = "";
		while (getline(classNamesFile, className)) classNamesVec.push_back(className);
	}


	if (cap.open("Project3_1.mp4") == 0) {
		cout << "NO FILE" << endl;
		return 0;
	}

	Mat frame;
	double fps = cap.get(CAP_PROP_FPS);
	int delay = 1000 / fps;

	
	Rect rect_left(80, 300, 220, 180);
	Rect rect_right(420, 300, 220, 180);
	Rect rect_middle(300, 300, 120, 180);

	bool people = false;
	bool car = false;
	bool front = false;
	bool lane = false;


	while (true) {
		cap >> frame;
		if (frame.empty()) break;
		if ((int)cap.get(CAP_PROP_POS_FRAMES) % 10 != 1) {
		
			imshow("Project3", SENSOR(frame,car, people, front, lane));
			waitKey(delay); 
			continue; 
		
		}

		people = false;
		car = false;
		front = false;
		lane = false;

		//Lane departure
		canny_left = frame(rect_left);
		canny_right = frame(rect_right);
		canny_middle = frame(rect_middle);

		cvtColor(canny_left, canny_left, CV_BGR2GRAY);
		blur(canny_left, canny_left, Size(5, 5));
		Canny(canny_left, canny_left, 40, 200, 3);
		cvtColor(canny_right, canny_right, CV_BGR2GRAY);
		blur(canny_right, canny_right, Size(5, 5));
		Canny(canny_right, canny_right, 40, 200, 3);
		cvtColor(canny_middle, canny_middle, CV_BGR2GRAY);
		blur(canny_middle, canny_middle, Size(5, 5));
		Canny(canny_middle, canny_middle, 40, 100, 3);

		HoughLines(canny_left, lines1, 1, CV_PI / 180, 50, 0, 0, CV_PI / 6, CV_PI / 3);
		HoughLines(canny_right, lines2, 1, CV_PI / 180, 50, 0, 0, CV_PI / 3 * 2, CV_PI / 6*5);
		HoughLines(canny_middle, lines3, 1, CV_PI / 180, 50, 0, 0, CV_PI /4*3, CV_PI);
		HoughLines(canny_middle, lines4, 1, CV_PI / 180, 50, 0, 0, 0, CV_PI / 4);

		if ((lines3.size() || lines4.size()) && !(lines1.size() * lines2.size()))
			lane = true;


		//Start Moving
		//Car detected nearby
		if (frame.channels() == 4) cvtColor(frame, frame, COLOR_BGRA2BGR);

		Mat inputBlob = blobFromImage(frame, 1 / 255.F, Size(416, 416), Scalar(), true, false);
		net.setInput(inputBlob, "data");

		Mat detectionMat = net.forward("detection_out");

		

		float confidenceThreshold = 0.24;

		for (int i = 0; i < detectionMat.rows; i++) {
			const int probability_index = 5;
			const int probability_size = detectionMat.cols - probability_index;
			float* prob_array_ptr = &detectionMat.at<float>(i, probability_index);
			size_t objectClass = max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
			float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);
			if (confidence > confidenceThreshold) {
				float x_center = detectionMat.at<float>(i, 0) * frame.cols;
				float y_center = detectionMat.at<float>(i, 1) * frame.rows;
				float width = detectionMat.at<float>(i, 2) * frame.cols;
				float height = detectionMat.at<float>(i, 3) * frame.rows;

				Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
				Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
				Rect object(p1, p2);



				
				String className = objectClass < classNamesVec.size() ? classNamesVec[objectClass]:
					format("unknown(%d)", objectClass);

				if (className == "car") {
					rectangle(frame, object, object_roi_red);

					if (height > 80&&width >40)
						car = true;

					if (x_center <= 370 && x_center >= 310 && y_center <= 330 && y_center >= 270&&width>55&&height>55)
						front = true;

				}


				//Human detected nearby
				else if (className == "person") {
					rectangle(frame, object, object_roi_green);

					if (height > 90&&width >35)
						people = true;
				}

			}
		}








		SENSOR(frame, car, people,front, lane);

		imshow("Project3", frame);



		waitKey(delay);
	}

	





	return 0;
}

Mat SENSOR(Mat frame, bool car, bool people, bool front, bool lane) {

	if (car)
		putText(frame, format("Car detected Nearby!"), Point(110, 120), CV_FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 5);
	if (people)
		putText(frame, format("Human detected Nearby!"), Point(70, 180), CV_FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255,0), 5);
	if (front)
		putText(frame, format("Start Moving!"), Point(200, 240), CV_FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255,0,0), 5);
	if (lane)
		putText(frame, format("Lane departure!"), Point(180, 60), CV_FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 5);
	


	return frame;
}
