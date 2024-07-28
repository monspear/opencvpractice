#include <iostream>>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace std;
using namespace cv;

Point2i s(0, 0); // start point
Point2i e(0, 0); // end point


void MouseClick(int event, int x, int y, int flag, void* param)
{

	Mat c = *(Mat*)param; // 파라미터를 Mat*로 변환후 c에 param값을 넣는다.(원본이미지의 값, (mat*) 앞에 *을 붙이지 않으면 주소가 들어가기에 오류가 난다.)

	switch (event) // 이벤트 값으로 마우스이벤트의 상태를 본다.
	{
	case EVENT_LBUTTONDOWN: // 마우스 왼쪽버튼 누름
		s.x = x;
		s.y = y; // 마우스의 좌표(x, y)를 s에 각각 저장
		cout << x << "		" << y << '\n';
		break;
	case EVENT_LBUTTONUP: // 마우스 왼쪽버튼을 뗐을 때
		e.x = x;
		e.y = y; //마우스의 좌표(x, y)를 e에 각각 저장
		cout << x << "		" << y << '\n';
		rectangle(c, s, e, Scalar(0, 255, 0, 0), 3);// c이미지에 녹색의 사각형을 그린다. 선의 굵기는 3으로 지정
		if (s == e) { // 만약 같은 지점을 마우스로 눌렀다가 땠을 때 오류가 나면서 단번에 꺼지는데 이를 방지하기 위해 처리한 것이다.
			e.x = s.x + 1;
			e.y = s.y + 1;
		}
		Mat cvt(c, Rect(s, e)); // cvt 객체를 c의 이미지에서 s,e의 좌표만큼 사각형 만든 것으로 지정
		cvtColor(cvt, cvt, COLOR_BGR2GRAY); // cvt 객체의 이미지를 회색조로 저장
		imshow("이미지2", cvt);
		cvt.zeros(cvt.rows, cvt.cols, CV_8UC1); //cvt 객체 초기화
		break;
	}
	imshow("이미지1", c);

}

int main(void)
{
	Mat img = imread("./aa.jpg");

	imshow("이미지1", img);
	setMouseCallback("이미지1", MouseClick, &img); // 마우스 콜백 함수. "이미지1"에 마우스를 이용한 모든 이벤트에 대해 MouseClick() 함수가 실행된다. &img는 img를 파라미터로 전달. 

	waitKey();
	return 0;
}