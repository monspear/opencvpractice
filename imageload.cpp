#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// ���� img ���, W/H ���, 
void img_info(Mat img, String txt)
{
	imshow(txt, img);
	Size s = img.size();
	cout << txt + " size :" << s << endl << endl;
}

// ���� rt �� �»����ǥ, ���ϴ� ��ǥ ���
void coordinate(Rect rt)
{
	String txt1 = format("       top left(%d, %d) right bottom(%d, %d)",
		rt.tl().x, rt.tl().y, rt.br().x, rt.br().y);
	cout << txt1 << endl << endl;
	waitKey();
}

// img �� rt �簢�� ���� ���������� �׸��� 
void draw_rect(Mat img, Rect rt, String txt) {
	rectangle(img, rt, Scalar(0, 0, 255), 2);
	img_info(img, txt);
}
int main() {
	Mat img = imread("aa.jpg");		img_info(img, "ori img");


	// ���� �簢�� rt1 (50, 50, 200, 150)  �׸��� ���
	Rect rt1(50, 50, 200, 150);
	Mat img1 = img.clone();

	draw_rect(img1, rt1, "img_1");
	coordinate(rt1);

	// rt1 �� (100, 100) �̵�  ���� rt2 �� ���� �ش� �簢�� 
	// �׸��� ���

	Mat img2 = img.clone();
	Point2i p(100, 100);
	Rect rt2 = rt1 + p;

	draw_rect(img2, rt2, "img_2");
	coordinate(rt2);

	// rt1 �� (50, 50)�� Ȯ���� ���� rt3 �� ���� �ش� �簢�� 
	// �׸��� ���

	Mat img3 = img.clone();
	Size sz(50, 50);
	Rect rt3 = rt1 + sz;

	draw_rect(img3, rt3, "img_3");
	coordinate(rt3);

	// r1, r2 �������� rt4 �� �簢���� ���� �׸��� ���

	Mat img4 = img.clone();
	Rect rt4 = rt1 & rt2;

	draw_rect(img4, rt4, "img_4");
	coordinate(rt4);

	// r1, r2 ��� ���Կ��� rt5 �� �簢���� ���� �׸��� ���

	Mat img5 = img.clone();
	Rect rt5 = rt1 | rt2;

	draw_rect(img5, rt5, "img_5");
	coordinate(rt5);

	// �� ���� (100, 100) �߽����� size(100, 50) �簢���� 20�� ȸ���� 
	// �簢�� �׸��� ���

	Mat img6 = img.clone();
	Point2i center(100, 100);
	Point2f ptr[4];
	Size_<int> siz(100, 50);
	RotatedRect R_R(center, siz, 20); // ȸ���簢�� ����(�簢���� �߽�,�簢���� ũ��, ����)

	R_R.points(ptr); // ptr[4]�� ȸ���簢���� �������� ��ȯ
	for (int i = 0; i < 4; i++)
	{
		line(img, ptr[i], ptr[(i + 1) % 4], Scalar(0, 0, 255), 2); // (��Ÿ�� �̹���, ���ۼ���, ������, ����(B, G, R), ������ ����)
	}
	imshow("img_6", img);

	waitKey();
	return 0;
}