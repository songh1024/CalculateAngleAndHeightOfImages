#include"custom_function.h"
#include<opencv2\opencv.hpp>
#include <string>
#include<iostream>

using namespace std;
using namespace cv;

int main(int argc, int** argv) {
	string DATA_DIR = "images/";
	int start = 83,end = 95;
	string imageName;
	for (int i = start; i <= end; ++i) {
		imageName = "IMG_32" + to_string(i) + ".jpg";
		Mat srcImage = imread(DATA_DIR + imageName);
		if (srcImage.empty()) {
			cout << "Image not exist!";
			continue;
		}
		double height = findHeight(srcImage);
		double angle = findAngle(srcImage);
		cout << "ImageName: " << imageName << " | Height: " << 
			fixed << setprecision(2)<<
			height << " | Angle: " << angle << endl;
	}
	return 0;
}

