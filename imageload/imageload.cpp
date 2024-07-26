#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// 영상 img 출력, W/H 출력, 
void img_info(Mat img, String txt)
{
	imshow(txt, img);
	Size s = img.size();
	cout << txt + " size :" << s << endl << endl;
}

// 영역 rt 의 좌상단좌표, 우하단 좌표 출력
void coordinate(Rect rt)
{
	String txt1 = format("       top left(%d, %d) right bottom(%d, %d)",
		rt.tl().x, rt.tl().y, rt.br().x, rt.br().y);
	cout << txt1 << endl << endl;
	waitKey();
}

// img 에 rt 사각형 영역 빨강색으로 그리기 
void draw_rect(Mat img, Rect rt, String txt) {
	rectangle(img, rt, Scalar(0, 0, 255), 2);
	img_info(img, txt);
}
int main() {
	Mat img = imread("aa.jpg");		img_info(img, "ori img");


	// 영상에 사각형 rt1 (50, 50, 200, 150)  그리고 출력
	Rect rt1(50, 50, 200, 150);
	Mat img1 = img.clone();

	draw_rect(img1, rt1, "img_1");
	coordinate(rt1);

	// rt1 을 (100, 100) 이동  영역 rt2 를 영상에 해당 사각형 
	// 그리고 출력

	Mat img2 = img.clone();
	Point2i p(100, 100);
	Rect rt2 = rt1 + p;

	draw_rect(img2, rt2, "img_2");
	coordinate(rt2);

	// rt1 을 (50, 50)씩 확장한 영역 rt3 을 영상에 해당 사각형 
	// 그리고 출력

	Mat img3 = img.clone();
	Size sz(50, 50);
	Rect rt3 = rt1 + sz;

	draw_rect(img3, rt3, "img_3");
	coordinate(rt3);

	// r1, r2 교차영역 rt4 의 사각형을 영상에 그리고 출력

	Mat img4 = img.clone();
	Rect rt4 = rt1 & rt2;

	draw_rect(img4, rt4, "img_4");
	coordinate(rt4);

	// r1, r2 모두 포함영역 rt5 의 사각형을 영상에 그리고 출력

	Mat img5 = img.clone();
	Rect rt5 = rt1 | rt2;

	draw_rect(img5, rt5, "img_5");
	coordinate(rt5);

	// 원 영상에 (100, 100) 중심으로 size(100, 50) 사각형을 20도 회전한 
	// 사각형 그리고 출력

	Mat img6 = img.clone();
	Point2i center(100, 100);
	Point2f ptr[4];
	Size_<int> siz(100, 50);
	RotatedRect R_R(center, siz, 20); // 회전사각형 선언(사각형의 중심,사각형의 크기, 각도)

	R_R.points(ptr); // ptr[4]에 회전사각형의 꼭짓점을 반환
	for (int i = 0; i < 4; i++)
	{
		line(img, ptr[i], ptr[(i + 1) % 4], Scalar(0, 0, 255), 2); // (나타낼 이미지, 시작선분, 끝선분, 색깔(B, G, R), 선분의 굵기)
	}
	imshow("img_6", img);

	waitKey();
	return 0;
}