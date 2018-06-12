#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

double findHeight(Mat &srcImage)
/*���룺ԭͼ��
**�����ͼ��߶ȣ���������򷵻�-1
*/
{
	//��ʱ��������   
	Mat dstImage, blurImage, threshImage, edgeImage, g_grayImage, lineImage;
	Mat ROIImage = srcImage(Rect(1850, 610, 210, 100));  //�ü�С��������
	cvtColor(ROIImage, g_grayImage, COLOR_BGR2GRAY); //תΪ�Ҷ�ͼ
	GaussianBlur(g_grayImage, blurImage, Size(9, 9), 0, 0);//ģ������
	threshold(blurImage, threshImage, 130, 255, THRESH_BINARY_INV); //��ֵ
	//ģ��(���ٽǵ�����)
	GaussianBlur(threshImage, threshImage, Size(3, 3), 0, 0);

	//�����ؼ��ǵ���
	vector<Point2f> corners;  //��������Ľǵ�
	int g_maxCornerNumber = 40;//���ǵ�����
	double qualityLevel = 0.01;//�ǵ���ɽ��ܵ���С����ֵ
	double minDistance = 2;//�ǵ�֮�����С����
	int blockSize = 3;//���㵼������ؾ���ʱָ��������Χ
	double k = 0.04;  //Ȩ��ϵ��
	
	goodFeaturesToTrack(threshImage, corners, g_maxCornerNumber, qualityLevel, minDistance, Mat(), blockSize, true, k);
	
	// cout << "\n\t>-------------�˴μ�⵽�Ľǵ�����Ϊ��" << corners.size() << endl;

	//���Ƽ�⵽�Ľǵ�
	Mat copy = ROIImage.clone();//����Դͼ��һ����ʱ�����У���Ϊ����Ȥ����,������ͼ
								//�ǵ��⺯��
	RNG g_rng(12345);//��ʼ������������������������ɫ
	int r = 4;//��뾶
	for (unsigned int i = 0; i < corners.size(); i++)
	{
		//���������ɫ���Ƴ��ǵ�
		circle(copy, corners[i], r, Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255),
			g_rng.uniform(0, 255)), -1, 8, 0);
	}

	Size winSize = Size(5, 5);
	Size zeroZone = Size(-1, -1);
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
	//����������ؽǵ�λ��
	cornerSubPix(g_grayImage, corners, winSize, zeroZone, criteria);
	//����ǵ���Ϣ
	vector<float> corners_y;
	for (int i = 0; i < corners.size(); i++)
		corners_y.push_back(corners[i].y); //����ÿ���ǵ�ĸ߶�

	if (corners_y.size() <= 0) {
		cout << "δ�ҵ��ؼ���" << endl;
		return -1;
	}

	float max = corners_y[0];
	float min = corners_y[0];
	for (int i = 0; i < corners_y.size(); ++i) {
		max = max > corners_y[i] ? max : corners_y[i];
		min = min < corners_y[i] ? min : corners_y[i];
	}
	//��һ��
	for (int i = 0; i < corners_y.size(); ++i) {
		corners_y[i] = (corners_y[i] - min) / (max - min);
	}
	//����㣬������kmeans
	Mat points(corners_y.size(), 1, CV_32FC2), labels;
	int row = points.rows;
	int col = points.cols*points.channels();//ÿһ�е�Ԫ����
	for (int i = 0; i < row; ++i) {
		float *data = points.ptr<float>(i);
		data[0] = corners_y[i];
		data[1] = 0;
	}
	// cout << points << endl;
	Mat centers(1, 1, points.type());    //�����洢���������ĵ�
	kmeans(points, 1, labels,
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, KMEANS_RANDOM_CENTERS, centers);
	float kmeans_center = centers.at<Vec2f>(0, 0)[0];  //������
	float original_center = kmeans_center*(max - min) + min;  //ȥ��һ������ԭԭʼ��
															  //���ݾ������Ļ�����ʾ��ȵ�ֱ��
	//����ֱ��
	Point pt1(0, original_center);
	Point pt2(copy.cols, original_center);
	line(copy, pt1, pt2, Scalar(0, 255, 0), 2);

	/*ͼ����ʾ
	namedWindow("ԭͼ", CV_WINDOW_KEEPRATIO);
	namedWindow("�ָ�ͼ", CV_WINDOW_KEEPRATIO);
	namedWindow("ԭͼ", CV_WINDOW_KEEPRATIO);
	imshow("ԭͼ", ROIImage);
	imshow("�ָ�ͼ", threshImage);
	imshow("�߶�", copy);
	waitKey(0);
	*/
	return original_center;
}

double findAngle(Mat& srcImage) 
/*���룺ԭͼ��
**������Ƕ�
*/
{
	//����һЩ��ʱ����
	Mat blurImage, edgeImage, g_grayImage, lineImage, thresholdImage;
	
	Mat ROIImage = srcImage(Rect(1850, 615, 210, 40)); //�ü�С��������
	cvtColor(ROIImage, g_grayImage, COLOR_BGR2GRAY); //תΪ�Ҷ�ͼ
	GaussianBlur(g_grayImage, blurImage, Size(9, 9), 0, 0); //ģ������
	threshold(blurImage, thresholdImage, 150, 255, THRESH_BINARY_INV); //��ֵ
	Canny(thresholdImage, edgeImage, 50, 200, 3); //��Ե���

	vector<Vec4i> lines;//����һ��ʸ���ṹlines���ڴ�ŵõ����߶�ʸ������
						//����任����ֱ��
	HoughLinesP(edgeImage, lines, 1, CV_PI / 180, 10, 15, 10);

	//������ͼ�л��Ƴ�ÿ���߶�
	cvtColor(edgeImage, lineImage, COLOR_GRAY2BGR);
	// cout << "ֱ������" << lines.size() << endl;
	vector<float> ang1, ang2;
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		// cout << "(" << l[0] << "," << l[1] << ")" << "(" << l[2] << "," << l[3] << ")";
		line(lineImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, LINE_AA);
		//����ÿһ���ߵĽǶ�
		float angle = atan((l[3] - l[1]) / float(l[2] - l[0])) * 180 / CV_PI;
		// cout << " angle = " << angle << endl;
		//���������Ƕ�
		if (angle > 0)
			ang1.push_back(angle);
		else
			ang2.push_back(angle);
	}
	double avg_ang1 = 0, avg_ang2 = 0;
	//��Ƕ�ƽ��ֵ
	for (int i = 0; i < ang1.size(); ++i)
		avg_ang1 += ang1[i];
	avg_ang1 = avg_ang1 / ang1.size();
	for (int i = 0; i < ang2.size(); ++i)
		avg_ang2 += ang2[i];
	avg_ang2 = avg_ang2 / ang2.size();
	//�������սǶ�
	double finalAngle = 180 - abs(avg_ang1) - abs(avg_ang1);
	// cout << "\nFinalAngle:" << finalAngle << endl;
	/*
	imshow("ԭͼ", ROIImage);
	imshow("��ֵ", thresholdImage);
	imshow("��Ե", edgeImage);
	imshow("ֱ��", lineImage);
	waitKey(0);
	*/
	return finalAngle;
}
