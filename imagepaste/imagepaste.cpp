#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void f21() {



	Mat img = imread("ex_color.jpg"); // img �� cpp���ϰ� ���� ��ġ�� �ִ� "ex_color.jpg"�� �о�ͼ� ����

	Mat logo = imread("logo.jpg");    // logo �� cpp���Ͽ� ���� ��ġ�� �ִ� "logo.jpg"�� �о�ͼ� ����



	logo.copyTo(img(Rect(30, 40, logo.cols, logo.rows))); // logo �� img ���� �»��(0,0) ���� 30, 40 ��ġ�� ������.(ũ��� logo �� ��,�� ũ���.) 
	//�� ������ logo �»�� ��ġ��(30, 40), ���ϴ� ��ġ��(30 + logo.cols, 40 + logo.rows);


	imshow("frame1", img); // 'frame1' �̶�� ������ ���� ������ â�� img �� ��Ÿ��. 


	waitKey(0); // �Ű������� 0�� ��� ���� �Է��� ������ ���� ��ٸ�
	// 0�� �ƴ� ������ ��� �� ����(���� : ms) ��ŭ ��ٸ� �� ���� ������ ����.(ex. waitKey(5000) -> 5�� �� ���� ���� ����) 
}



void f22() {

	Mat img = imread("ex_color.jpg"); // img �� cpp���ϰ� ���� ��ġ�� �ִ� "ex_color.jpg"�� �о�ͼ� ����
	Mat logo = imread("logo.jpg");    // logo �� cpp���Ͽ� ���� ��ġ�� �ִ� "logo.jpg"�� �о�ͼ� ����



	Mat mask; // Mat ��ü ����

	cvtColor(logo, mask, COLOR_BGR2GRAY); // mask �� logo�� ��(b,g,r)�� ȸ������ ��ȯ�� ����� �־��ش�.



	inRange(mask, 10, 255, mask); // mask�� ������ 10�̻� 255������ ���� ������� ��Ÿ���ϴ�. ������ ���� �ȿ� ���� ������ ������(0)���� ��Ÿ���� �̰��� mask�� �����մϴ�.

	imshow("mask", mask); // "mask" �� �̸��� ���� ������ â�� mask�� ��Ÿ����



	Mat frameROI = img(Rect(30, 40, logo.cols, logo.rows)); // frameROI �� img�� �»��(30,40), ���ϴ� (30 + logo�� ��, 40 + logo�� ��) ��ŭ�� ���� ���縦 �Ѵ�.

	imshow("ROI", frameROI); //'frameROI' �̶�� ������ ���� ������ â�� img �� ��Ÿ��.



	logo.copyTo(frameROI, mask); // logo �� frameROI ���� �����ϴµ� mask�� ���Ұ��� 0�� �ƴ� ��ǥ������ ��� ���Ҹ� �����մϴ�.



	imshow("frame2", img); //'frame2' �̶�� ������ ���� ������ â�� img �� ��Ÿ��.

	waitKey(0); //  ���� �Է��� ������ ���� ��ٸ�

}



int main() {

	f21();

	f22();

	return 0;

}