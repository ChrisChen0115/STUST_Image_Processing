#include "qtwidgetsapplication1.h"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

//開啟檔案
void QtWidgetsApplication1::openFile()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("選擇"), "D:/Images", tr("Images(*.png *.jpg *.jpeg *.gif)"));
	if (filename != 0) {
		QImage image;
		bool valid = image.load(filename);
		if (valid) {
			image = image.scaledToWidth(ui.label_Image->width(), Qt::SmoothTransformation);
			ui.label_Image->setPixmap(QPixmap::fromImage(image));
		}
		ui.image_size_label_width->setText(QString::number(image.width()));
		ui.image_size_label_height->setText(QString::number(image.height()));
	}
}

//ROI
void QtWidgetsApplication1::ROI()
{
	int x1, x2, y1, y2;
	QString X1 = ui.lineEdit_X1->text();
	QString X2 = ui.lineEdit_X2->text();
	QString Y1 = ui.lineEdit_Y1->text();
	QString Y2 = ui.lineEdit_Y2->text();
	//empty input
	if (X1 == NULL || X2 == NULL || Y1 == NULL || Y2 == NULL) {
		QMessageBox::warning(NULL, "Error!", "Wrong Value");
	}
	else {
		x1 = X1.toInt();
		x2 = X2.toInt();
		y1 = Y1.toInt();
		y2 = Y2.toInt();
		if (x1 > x2 || y1 > y2) {
			QMessageBox::warning(NULL, "Error!", "Wrong Value");
		}
		else {
			QImage image;
			const QPixmap* pixmap = ui.label_Image->pixmap();
			if (pixmap)
			{
				QImage image1(pixmap->toImage());
				image = image1;
			}
			//out of height
			if (x2 > image.height()) {
				QMessageBox::warning(NULL, "out of height!", "Wrong x2 value!");
			}
			//out of width
			else if (y2 > image.width()) {
				QMessageBox::warning(NULL, "out of width!", "Wrong y2 value!");
			}
			else {
				//QImage -> Mat
				Mat mat = QImage2cvMat(image);
				Mat imageROI = mat(Range(x1, x2), Range(y1, y2));
				imshow("ROI Image", imageROI);
				waitKey(0);
				destroyAllWindows();
			}
		}
	}
}

//繪製直方圖
void QtWidgetsApplication1::histogram() {
	//轉灰階
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_src;
	cvtColor(src, Gray_src, COLOR_BGR2GRAY);
	imshow("Original image", Gray_src);
	if (!Gray_src.data) { QMessageBox::warning(NULL, "Error!", "Wrong image!"); }

	Mat dstHist;
	int dims = 1;
	float hranges[] = { 0, 256 };
	const float* ranges[] = { hranges };
	int size = 256;
	int channels = 0;

	calcHist(&Gray_src, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstImage(size, size, CV_8U, Scalar(0));
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);


	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);

		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		line(dstImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("Result", dstImage);
	waitKey(0);
}

//轉為灰階
void QtWidgetsApplication1::change2Gray() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2GRAY);
	imshow("Origin Image", src);
	imshow("Gray Image", dst);
	waitKey(0);
	destroyAllWindows();
}

//轉為HSV
void QtWidgetsApplication1::change2HSV() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2HSV);
	imshow("Origin Image", src);
	imshow("HSV Image", dst);
	waitKey(0);
	destroyAllWindows();
}

//二值化
void QtWidgetsApplication1::Qthreshold() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_src;
	int tvalue = ui.thresholdSlider->value();
	cvtColor(src, Gray_src, COLOR_BGR2GRAY);
	Mat dst;
	threshold(Gray_src, dst, tvalue, 255, THRESH_BINARY);
	imshow("Threshold Image", dst);
}

//直方圖等化
void QtWidgetsApplication1::QequalizeHist() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image, result;
	cvtColor(src, Gray_image, COLOR_BGR2GRAY);
	equalizeHist(Gray_image, result);

	Mat dstHist;
	int dims = 1;
	float hranges[] = { 0, 255 };
	const float* ranges[] = { hranges };
	int size = 256;
	int channels = 0;

	//計算影像的直方圖
	calcHist(&result, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstHistImage(size, size, CV_8U, Scalar(0));
	//獲得最小值最大值
	double minValue = 0;
	double dstMaxValue = 0;
	minMaxLoc(dstHist, &minValue, &dstMaxValue, 0, 0); //在cv中用的是cvGetMinMaxHistValue

	//繪製直方圖
	//saturate_cast函數的作用即是：當運算完之後，最小值0最大值為255。
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);//hist類別為float 
		//拉伸到0-max
		int realValue = saturate_cast<int>(binValue * hpt / dstMaxValue);
		line(dstHistImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("Original Image", Gray_image);
	imshow("直方圖等化", result);
	imshow("直方圖", dstHistImage);
	waitKey(0);
	destroyAllWindows();
}

//高斯平滑
void QtWidgetsApplication1::QgaussianBlur() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	GaussianBlur(src, dst, Size(11, 11), 0, 0);
	imshow("Oringin", src);
	imshow("GaussianBlur", dst);
	waitKey(0);
	destroyAllWindows();
}

//中值平滑
void QtWidgetsApplication1::QmedianBlur() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	medianBlur(src, dst, 7);
	imshow("Oringin", src);
	imshow("MedianBlur", dst);
	waitKey(0);
	destroyAllWindows();
}

//雙邊平滑
void QtWidgetsApplication1::QbilateralFilter() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	bilateralFilter(src, dst, 9, 75, 75, BORDER_DEFAULT);
	imshow("Oringin", src);
	imshow("BilateralFilter", dst);
	waitKey(0);
	destroyAllWindows();
}

//平移
void QtWidgetsApplication1::QwarpAffine() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	Size dst_sz = src.size();
	Mat t_mat = Mat::zeros(2, 3, CV_32FC1);
	/*
	[1 0 tx]
	[0 1 ty]
	*/
	int x, y;
	QString X = ui.warpAffine_X_text->text();
	QString Y = ui.warpAffine_Y_text->text();
	x = X.toInt();
	y = Y.toInt();
	t_mat.at<float>(0, 0) = 1;
	t_mat.at<float>(0, 2) = x; //水平
	t_mat.at<float>(1, 1) = 1;
	t_mat.at<float>(1, 2) = y; //垂直

	warpAffine(src, dst, t_mat, dst_sz);
	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

//旋轉
void QtWidgetsApplication1::Qrotate() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	QString Angle = ui.rotate_text->text();
	//旋轉角度
	double angle = Angle.toDouble();
	Size src_sz = src.size();
	Size dst_sz(src_sz.height, src_sz.width);
	int len = max(src.cols, src.rows);
	//旋轉中心
	Point2f center(len / 2., len / 2.);
	//旋轉矩陣
	Mat rot_mat = getRotationMatrix2D(center, angle, 1.0);
	//根據旋轉矩陣進行Affine
	warpAffine(src, dst, rot_mat, dst_sz);

	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

//自訂參數 - 仿射轉換
void QtWidgetsApplication1::QaffineTransform() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;

	Point2f srcPoints[3];//原圖中的三點 ,一個包含三維點（x，y）的數組，其中x、y是浮點型數
	Point2f dstPoints[3];//目標圖中的三點  

	//原始圖像的三個座標 
	srcPoints[0] = Point2f(0, 0);//原點
	srcPoints[1] = Point2f(0, src.rows);//寬
	srcPoints[2] = Point2f(src.cols, 0);//長
	//映射後的三個座標值
	QString dst0_X = ui.affineTransform_text_1->text();
	QString dst0_Y = ui.affineTransform_text_2->text();
	QString dst1_X = ui.affineTransform_text_3->text();
	QString dst1_Y = ui.affineTransform_text_4->text();
	QString dst2_X = ui.affineTransform_text_5->text();
	QString dst2_Y = ui.affineTransform_text_6->text();
	double dst0_x, dst0_y, dst1_x, dst1_y, dst2_x, dst2_y;
	dst0_x = dst0_X.toDouble();
	dst0_y = dst0_Y.toDouble();
	dst1_x = dst1_X.toDouble();
	dst1_y = dst1_Y.toDouble();
	dst2_x = dst2_X.toDouble();
	dst2_y = dst2_Y.toDouble();


	dstPoints[0] = Point2f(src.cols * dst0_x, src.rows * dst0_y);
	dstPoints[1] = Point2f(src.cols * dst1_x, src.rows * dst1_y);
	dstPoints[2] = Point2f(src.cols * dst2_x, src.rows * dst2_y);

	Mat M1 = getAffineTransform(srcPoints, dstPoints);//由三個點對計算變換矩陣  
	warpAffine(src, dst, M1, src.size());//仿射變換
	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

int i = 0;//索引值
float pArray[4][2];//4個點，內有X,Y值
void QtWidgetsApplication1::mousePressEvent(QMouseEvent* event) {
	QPoint pCursor = event->pos();
	QString position = QString("%1,%2").arg(pCursor.x()).arg(pCursor.y());
	//第5次選取時，會回到第一筆陣列並覆蓋
	if (i == 4) i = 0;
	pArray[i][0] = pCursor.x();
	pArray[i][1] = pCursor.y();
	i += 1;
}

//透視投影變換
void QtWidgetsApplication1::QperspectiveTransform() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;

	Point2f srcPoints[4];//原圖中的四點
	Point2f dstPoints[4];//目標圖中的四點  

	srcPoints[0] = Point2f(0, 0);//原點
	srcPoints[1] = Point2f(0, src.rows);//寬
	srcPoints[2] = Point2f(src.cols, 0);//長
	srcPoints[3] = Point2f(src.cols, src.rows);//對角頂點

	dstPoints[0] = Point2f(pArray[0][0] - 11, pArray[0][1] - 65);
	dstPoints[1] = Point2f(pArray[1][0] - 11, pArray[1][1] - 65);
	dstPoints[2] = Point2f(pArray[2][0] - 11, pArray[2][1] - 65);
	dstPoints[3] = Point2f(pArray[3][0] - 11, pArray[3][1] - 65);
	Mat M1 = getPerspectiveTransform(srcPoints, dstPoints);//由四個點對計算變換矩陣  
	warpPerspective(src, dst, M1, src.size());
	imshow("result", dst);
	waitKey(0);
}

//Mat轉QImage
QImage cvMat2QImage(const Mat& mat) {
	{
		if (mat.type() == CV_8UC1)                          // 單通道
		{
			QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
			image.setColorCount(256);                       // 灰度級數256
			for (int i = 0; i < 256; i)
			{
				image.setColor(i, qRgb(i, i, i));
			}
			uchar* pSrc = mat.data;                         // 複製mat資料
			for (int row = 0; row < mat.rows; row)
			{
				uchar* pDest = image.scanLine(row);
				memcpy(pDest, pSrc, mat.cols);
				pSrc = mat.data;
			}
			return image;
		}
		else if (mat.type() == CV_8UC3)                     // 3通道
		{
			const uchar* pSrc = (const uchar*)mat.data;     // 複製畫素
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);    // R, G, B 對應 0,1,2
			return image.rgbSwapped();                      // rgbSwapped是為了顯示效果色彩好一些。
		}
		else if (mat.type() == CV_8UC4)                     // 4通道
		{
			const uchar* pSrc = (const uchar*)mat.data;     // 複製畫素
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);        // B,G,R,A 對應 0,1,2,3
			return image.copy();
		}
		else
		{
			return QImage();
		}
	}
}

//開啟鏡頭
void QtWidgetsApplication1::openCap() {
	VideoCapture cap(0);
	Mat frame;
	while (true) {
		// 擷取影像
		cap >> frame;
		//在QLabel上顯示視訊畫面
		QImage img = cvMat2QImage(frame);
		ui.label_Image->setPixmap(QPixmap::fromImage(img));
		imshow("cap", frame);
		//按下esc離開
		if (waitKey(1) == 27) {
			destroyAllWindows();
			break;
		}
	}
	cap.release();
	ui.label_Image->clear();
}