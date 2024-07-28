#include <iostream>>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace std;
using namespace cv;

Point2i s(0, 0); // start point
Point2i e(0, 0); // end point


void MouseClick(int event, int x, int y, int flag, void* param)
{

	Mat c = *(Mat*)param; // �Ķ���͸� Mat*�� ��ȯ�� c�� param���� �ִ´�.(�����̹����� ��, (mat*) �տ� *�� ������ ������ �ּҰ� ���⿡ ������ ����.)

	switch (event) // �̺�Ʈ ������ ���콺�̺�Ʈ�� ���¸� ����.
	{
	case EVENT_LBUTTONDOWN: // ���콺 ���ʹ�ư ����
		s.x = x;
		s.y = y; // ���콺�� ��ǥ(x, y)�� s�� ���� ����
		cout << x << "		" << y << '\n';
		break;
	case EVENT_LBUTTONUP: // ���콺 ���ʹ�ư�� ���� ��
		e.x = x;
		e.y = y; //���콺�� ��ǥ(x, y)�� e�� ���� ����
		cout << x << "		" << y << '\n';
		rectangle(c, s, e, Scalar(0, 255, 0, 0), 3);// c�̹����� ����� �簢���� �׸���. ���� ����� 3���� ����
		if (s == e) { // ���� ���� ������ ���콺�� �����ٰ� ���� �� ������ ���鼭 �ܹ��� �����µ� �̸� �����ϱ� ���� ó���� ���̴�.
			e.x = s.x + 1;
			e.y = s.y + 1;
		}
		Mat cvt(c, Rect(s, e)); // cvt ��ü�� c�� �̹������� s,e�� ��ǥ��ŭ �簢�� ���� ������ ����
		cvtColor(cvt, cvt, COLOR_BGR2GRAY); // cvt ��ü�� �̹����� ȸ������ ����
		imshow("�̹���2", cvt);
		cvt.zeros(cvt.rows, cvt.cols, CV_8UC1); //cvt ��ü �ʱ�ȭ
		break;
	}
	imshow("�̹���1", c);

}

int main(void)
{
	Mat img = imread("./aa.jpg");

	imshow("�̹���1", img);
	setMouseCallback("�̹���1", MouseClick, &img); // ���콺 �ݹ� �Լ�. "�̹���1"�� ���콺�� �̿��� ��� �̺�Ʈ�� ���� MouseClick() �Լ��� ����ȴ�. &img�� img�� �Ķ���ͷ� ����. 

	waitKey();
	return 0;
}