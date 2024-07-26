#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void f21() {



	Mat img = imread("ex_color.jpg"); // img 에 cpp파일과 같은 위치에 있는 "ex_color.jpg"를 읽어와서 저장

	Mat logo = imread("logo.jpg");    // logo 에 cpp파일에 같은 위치에 있는 "logo.jpg"를 읽어와서 저장



	logo.copyTo(img(Rect(30, 40, logo.cols, logo.rows))); // logo 를 img 위에 좌상단(0,0) 기준 30, 40 위치에 복사함.(크기는 logo 의 행,렬 크기다.) 
	//즉 복사한 logo 좌상단 위치는(30, 40), 우하단 위치는(30 + logo.cols, 40 + logo.rows);


	imshow("frame1", img); // 'frame1' 이라는 제목을 가진 윈도우 창에 img 를 나타냄. 


	waitKey(0); // 매개변수가 0인 경우 다음 입력이 있을때 까지 기다림
	// 0이 아닌 숫자인 경우 그 숫자(단위 : ms) 만큼 기다린 뒤 다음 동작을 실행.(ex. waitKey(5000) -> 5초 뒤 다음 동작 실행) 
}



void f22() {

	Mat img = imread("ex_color.jpg"); // img 에 cpp파일과 같은 위치에 있는 "ex_color.jpg"를 읽어와서 저장
	Mat logo = imread("logo.jpg");    // logo 에 cpp파일에 같은 위치에 있는 "logo.jpg"를 읽어와서 저장



	Mat mask; // Mat 객체 선언

	cvtColor(logo, mask, COLOR_BGR2GRAY); // mask 에 logo의 색(b,g,r)을 회색으로 전환한 모습을 넣어준다.



	inRange(mask, 10, 255, mask); // mask의 색상중 10이상 255이하의 값을 흰색으로 나타냅니다. 색상이 범위 안에 들지 않으면 검은색(0)으로 나타내며 이것을 mask에 저장합니다.

	imshow("mask", mask); // "mask" 란 이름을 가진 윈도우 창에 mask를 나타낸다



	Mat frameROI = img(Rect(30, 40, logo.cols, logo.rows)); // frameROI 는 img의 좌상단(30,40), 우하단 (30 + logo의 열, 40 + logo의 행) 만큼을 얕은 복사를 한다.

	imshow("ROI", frameROI); //'frameROI' 이라는 제목을 가진 윈도우 창에 img 를 나타냄.



	logo.copyTo(frameROI, mask); // logo 를 frameROI 위에 복사하는데 mask의 원소값이 0이 아닌 좌표에서만 행렬 원소를 복사합니다.



	imshow("frame2", img); //'frame2' 이라는 제목을 가진 윈도우 창에 img 를 나타냄.

	waitKey(0); //  다음 입력이 있을때 까지 기다림

}



int main() {

	f21();

	f22();

	return 0;

}