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

//�}���ɮ�
void QtWidgetsApplication1::openFile()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("���"), "D:/Images", tr("Images(*.png *.jpg *.jpeg *.gif)"));
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

//ø�s�����
void QtWidgetsApplication1::histogram() {
	//��Ƕ�
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

//�ର�Ƕ�
void QtWidgetsApplication1::change2Gray() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2GRAY);
	imshow("Origin Image", src);
	imshow("Gray Image", dst);
	waitKey(0);
	destroyAllWindows();
}

//�ରHSV
void QtWidgetsApplication1::change2HSV() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2HSV);
	imshow("Origin Image", src);
	imshow("HSV Image", dst);
	waitKey(0);
	destroyAllWindows();
}

//�G�Ȥ�
void QtWidgetsApplication1::Qthreshold() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_src;
	int tvalue = ui.thresholdSlider->value();
	cvtColor(src, Gray_src, COLOR_BGR2GRAY);
	Mat dst;
	threshold(Gray_src, dst, tvalue, 255, THRESH_BINARY);
	imshow("Threshold Image", dst);
}

//����ϵ���
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

	//�p��v���������
	calcHist(&result, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstHistImage(size, size, CV_8U, Scalar(0));
	//��o�̤p�ȳ̤j��
	double minValue = 0;
	double dstMaxValue = 0;
	minMaxLoc(dstHist, &minValue, &dstMaxValue, 0, 0); //�bcv���Ϊ��OcvGetMinMaxHistValue

	//ø�s�����
	//saturate_cast��ƪ��@�ΧY�O�G��B�⧹����A�̤p��0�̤j�Ȭ�255�C
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);//hist���O��float 
		//�Ԧ���0-max
		int realValue = saturate_cast<int>(binValue * hpt / dstMaxValue);
		line(dstHistImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("Original Image", Gray_image);
	imshow("����ϵ���", result);
	imshow("�����", dstHistImage);
	waitKey(0);
	destroyAllWindows();
}

//��������
void QtWidgetsApplication1::QgaussianBlur() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	GaussianBlur(src, dst, Size(11, 11), 0, 0);
	imshow("Oringin", src);
	imshow("GaussianBlur", dst);
	waitKey(0);
	destroyAllWindows();
}

//���ȥ���
void QtWidgetsApplication1::QmedianBlur() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	medianBlur(src, dst, 7);
	imshow("Oringin", src);
	imshow("MedianBlur", dst);
	waitKey(0);
	destroyAllWindows();
}

//���䥭��
void QtWidgetsApplication1::QbilateralFilter() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	bilateralFilter(src, dst, 9, 75, 75, BORDER_DEFAULT);
	imshow("Oringin", src);
	imshow("BilateralFilter", dst);
	waitKey(0);
	destroyAllWindows();
}

//����
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
	t_mat.at<float>(0, 2) = x; //����
	t_mat.at<float>(1, 1) = 1;
	t_mat.at<float>(1, 2) = y; //����

	warpAffine(src, dst, t_mat, dst_sz);
	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

//����
void QtWidgetsApplication1::Qrotate() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;
	QString Angle = ui.rotate_text->text();
	//���ਤ��
	double angle = Angle.toDouble();
	Size src_sz = src.size();
	Size dst_sz(src_sz.height, src_sz.width);
	int len = max(src.cols, src.rows);
	//���त��
	Point2f center(len / 2., len / 2.);
	//����x�}
	Mat rot_mat = getRotationMatrix2D(center, angle, 1.0);
	//�ھڱ���x�}�i��Affine
	warpAffine(src, dst, rot_mat, dst_sz);

	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

//�ۭq�Ѽ� - ��g�ഫ
void QtWidgetsApplication1::QaffineTransform() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;

	Point2f srcPoints[3];//��Ϥ����T�I ,�@�ӥ]�t�T���I�]x�Ay�^���ƲաA�䤤x�By�O�B�I����
	Point2f dstPoints[3];//�ؼйϤ����T�I  

	//��l�Ϲ����T�Ӯy�� 
	srcPoints[0] = Point2f(0, 0);//���I
	srcPoints[1] = Point2f(0, src.rows);//�e
	srcPoints[2] = Point2f(src.cols, 0);//��
	//�M�g�᪺�T�Ӯy�Э�
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

	Mat M1 = getAffineTransform(srcPoints, dstPoints);//�ѤT���I��p���ܴ��x�}  
	warpAffine(src, dst, M1, src.size());//��g�ܴ�
	imshow("image", src);
	imshow("result", dst);
	waitKey(0);
}

int i = 0;//���ޭ�
float pArray[4][2];//4���I�A����X,Y��
void QtWidgetsApplication1::mousePressEvent(QMouseEvent* event) {
	QPoint pCursor = event->pos();
	QString position = QString("%1,%2").arg(pCursor.x()).arg(pCursor.y());
	//��5������ɡA�|�^��Ĥ@���}�C���л\
	if (i == 4) i = 0;
	pArray[i][0] = pCursor.x();
	pArray[i][1] = pCursor.y();
	i += 1;
}

//�z����v�ܴ�
void QtWidgetsApplication1::QperspectiveTransform() {
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst;

	Point2f srcPoints[4];//��Ϥ����|�I
	Point2f dstPoints[4];//�ؼйϤ����|�I  

	srcPoints[0] = Point2f(0, 0);//���I
	srcPoints[1] = Point2f(0, src.rows);//�e
	srcPoints[2] = Point2f(src.cols, 0);//��
	srcPoints[3] = Point2f(src.cols, src.rows);//�﨤���I

	dstPoints[0] = Point2f(pArray[0][0] - 11, pArray[0][1] - 65);
	dstPoints[1] = Point2f(pArray[1][0] - 11, pArray[1][1] - 65);
	dstPoints[2] = Point2f(pArray[2][0] - 11, pArray[2][1] - 65);
	dstPoints[3] = Point2f(pArray[3][0] - 11, pArray[3][1] - 65);
	Mat M1 = getPerspectiveTransform(srcPoints, dstPoints);//�ѥ|���I��p���ܴ��x�}  
	warpPerspective(src, dst, M1, src.size());
	imshow("result", dst);
	waitKey(0);
}

//Mat��QImage
QImage cvMat2QImage(const Mat& mat) {
	{
		if (mat.type() == CV_8UC1)                          // ��q�D
		{
			QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
			image.setColorCount(256);                       // �ǫׯż�256
			for (int i = 0; i < 256; i)
			{
				image.setColor(i, qRgb(i, i, i));
			}
			uchar* pSrc = mat.data;                         // �ƻsmat���
			for (int row = 0; row < mat.rows; row)
			{
				uchar* pDest = image.scanLine(row);
				memcpy(pDest, pSrc, mat.cols);
				pSrc = mat.data;
			}
			return image;
		}
		else if (mat.type() == CV_8UC3)                     // 3�q�D
		{
			const uchar* pSrc = (const uchar*)mat.data;     // �ƻs�e��
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);    // R, G, B ���� 0,1,2
			return image.rgbSwapped();                      // rgbSwapped�O���F��ܮĪG��m�n�@�ǡC
		}
		else if (mat.type() == CV_8UC4)                     // 4�q�D
		{
			const uchar* pSrc = (const uchar*)mat.data;     // �ƻs�e��
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);        // B,G,R,A ���� 0,1,2,3
			return image.copy();
		}
		else
		{
			return QImage();
		}
	}
}

//�}�����Y
void QtWidgetsApplication1::openCap() {
	VideoCapture cap(0);
	Mat frame;
	while (true) {
		// �^���v��
		cap >> frame;
		//�bQLabel�W��ܵ��T�e��
		QImage img = cvMat2QImage(frame);
		ui.label_Image->setPixmap(QPixmap::fromImage(img));
		imshow("cap", frame);
		//���Uesc���}
		if (waitKey(1) == 27) {
			destroyAllWindows();
			break;
		}
	}
	cap.release();
	ui.label_Image->clear();
}