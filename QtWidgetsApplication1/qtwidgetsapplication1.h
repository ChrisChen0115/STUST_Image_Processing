#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtwidgetsapplication1.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>


class QtWidgetsApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtWidgetsApplication1(QWidget* parent = Q_NULLPTR);
	// QImage¬‡Mat
	cv::Mat QImage2cvMat(QImage image)
	{
		cv::Mat mat;
		switch (image.format())
		{
		case QImage::Format_ARGB32:
		case QImage::Format_RGB32:
		case QImage::Format_ARGB32_Premultiplied:
			mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
			break;
		case QImage::Format_RGB888:
			mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
			cv::cvtColor(mat, mat, CV_BGR2RGB);
			break;
		case QImage::Format_Indexed8:
			mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
			break;
		}
		return mat;
	}
	

protected:
	void mousePressEvent(QMouseEvent* event);
private:
	Ui::QtWidgetsApplication1Class ui;

private slots:
	void openFile();
	void ROI();
	void histogram();
	void Qthreshold();
	void QequalizeHist();
	void change2Gray();
	void change2HSV();
	void QgaussianBlur();
	void QmedianBlur();
	void QbilateralFilter();
	void QwarpAffine();
	void Qrotate();
	void QaffineTransform();
	void QperspectiveTransform();
	void openCap();
};
