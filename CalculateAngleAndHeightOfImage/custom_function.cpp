#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

double findHeight(Mat &srcImage)
/*输入：原图像
**输出：图像高度，计算错误则返回-1
*/
{
	//临时变量定义   
	Mat dstImage, blurImage, threshImage, edgeImage, g_grayImage, lineImage;
	Mat ROIImage = srcImage(Rect(1850, 610, 210, 100));  //裁剪小矩形区域
	cvtColor(ROIImage, g_grayImage, COLOR_BGR2GRAY); //转为灰度图
	GaussianBlur(g_grayImage, blurImage, Size(9, 9), 0, 0);//模糊处理
	threshold(blurImage, threshImage, 130, 255, THRESH_BINARY_INV); //阈值
	//模糊(减少角点数量)
	GaussianBlur(threshImage, threshImage, Size(3, 3), 0, 0);

	//亚像素级角点检测
	vector<Point2f> corners;  //保存检测出的角点
	int g_maxCornerNumber = 40;//最大角点数量
	double qualityLevel = 0.01;//角点检测可接受的最小特征值
	double minDistance = 2;//角点之间的最小距离
	int blockSize = 3;//计算导数自相关矩阵时指定的邻域范围
	double k = 0.04;  //权重系数
	
	goodFeaturesToTrack(threshImage, corners, g_maxCornerNumber, qualityLevel, minDistance, Mat(), blockSize, true, k);
	
	// cout << "\n\t>-------------此次检测到的角点数量为：" << corners.size() << endl;

	//绘制检测到的角点
	Mat copy = ROIImage.clone();//复制源图像到一个临时变量中，作为感兴趣区域,用来绘图
								//角点检测函数
	RNG g_rng(12345);//初始化随机数生成器，随机产生颜色
	int r = 4;//点半径
	for (unsigned int i = 0; i < corners.size(); i++)
	{
		//以随机的颜色绘制出角点
		circle(copy, corners[i], r, Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255),
			g_rng.uniform(0, 255)), -1, 8, 0);
	}

	Size winSize = Size(5, 5);
	Size zeroZone = Size(-1, -1);
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
	//计算出亚像素角点位置
	cornerSubPix(g_grayImage, corners, winSize, zeroZone, criteria);
	//输出角点信息
	vector<float> corners_y;
	for (int i = 0; i < corners.size(); i++)
		corners_y.push_back(corners[i].y); //保存每个角点的高度

	if (corners_y.size() <= 0) {
		cout << "未找到关键点" << endl;
		return -1;
	}

	float max = corners_y[0];
	float min = corners_y[0];
	for (int i = 0; i < corners_y.size(); ++i) {
		max = max > corners_y[i] ? max : corners_y[i];
		min = min < corners_y[i] ? min : corners_y[i];
	}
	//归一化
	for (int i = 0; i < corners_y.size(); ++i) {
		corners_y[i] = (corners_y[i] - min) / (max - min);
	}
	//构造点，用来做kmeans
	Mat points(corners_y.size(), 1, CV_32FC2), labels;
	int row = points.rows;
	int col = points.cols*points.channels();//每一行的元素数
	for (int i = 0; i < row; ++i) {
		float *data = points.ptr<float>(i);
		data[0] = corners_y[i];
		data[1] = 0;
	}
	// cout << points << endl;
	Mat centers(1, 1, points.type());    //用来存储聚类后的中心点
	kmeans(points, 1, labels,
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, KMEANS_RANDOM_CENTERS, centers);
	float kmeans_center = centers.at<Vec2f>(0, 0)[0];  //聚类结果
	float original_center = kmeans_center*(max - min) + min;  //去归一化，还原原始点
															  //根据聚类中心画出表示厚度的直线
	//画出直线
	Point pt1(0, original_center);
	Point pt2(copy.cols, original_center);
	line(copy, pt1, pt2, Scalar(0, 255, 0), 2);

	/*图像显示
	namedWindow("原图", CV_WINDOW_KEEPRATIO);
	namedWindow("分割图", CV_WINDOW_KEEPRATIO);
	namedWindow("原图", CV_WINDOW_KEEPRATIO);
	imshow("原图", ROIImage);
	imshow("分割图", threshImage);
	imshow("高度", copy);
	waitKey(0);
	*/
	return original_center;
}

double findAngle(Mat& srcImage) 
/*输入：原图像
**输出：角度
*/
{
	//创建一些临时变量
	Mat blurImage, edgeImage, g_grayImage, lineImage, thresholdImage;
	
	Mat ROIImage = srcImage(Rect(1850, 615, 210, 40)); //裁剪小矩形区域
	cvtColor(ROIImage, g_grayImage, COLOR_BGR2GRAY); //转为灰度图
	GaussianBlur(g_grayImage, blurImage, Size(9, 9), 0, 0); //模糊处理
	threshold(blurImage, thresholdImage, 150, 255, THRESH_BINARY_INV); //阈值
	Canny(thresholdImage, edgeImage, 50, 200, 3); //边缘检测

	vector<Vec4i> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合
						//霍夫变换，找直线
	HoughLinesP(edgeImage, lines, 1, CV_PI / 180, 10, 15, 10);

	//依次在图中绘制出每条线段
	cvtColor(edgeImage, lineImage, COLOR_GRAY2BGR);
	// cout << "直线数量" << lines.size() << endl;
	vector<float> ang1, ang2;
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		// cout << "(" << l[0] << "," << l[1] << ")" << "(" << l[2] << "," << l[3] << ")";
		line(lineImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, LINE_AA);
		//计算每一条线的角度
		float angle = atan((l[3] - l[1]) / float(l[2] - l[0])) * 180 / CV_PI;
		// cout << " angle = " << angle << endl;
		//保存两个角度
		if (angle > 0)
			ang1.push_back(angle);
		else
			ang2.push_back(angle);
	}
	double avg_ang1 = 0, avg_ang2 = 0;
	//求角度平均值
	for (int i = 0; i < ang1.size(); ++i)
		avg_ang1 += ang1[i];
	avg_ang1 = avg_ang1 / ang1.size();
	for (int i = 0; i < ang2.size(); ++i)
		avg_ang2 += ang2[i];
	avg_ang2 = avg_ang2 / ang2.size();
	//计算最终角度
	double finalAngle = 180 - abs(avg_ang1) - abs(avg_ang1);
	// cout << "\nFinalAngle:" << finalAngle << endl;
	/*
	imshow("原图", ROIImage);
	imshow("阈值", thresholdImage);
	imshow("边缘", edgeImage);
	imshow("直线", lineImage);
	waitKey(0);
	*/
	return finalAngle;
}
