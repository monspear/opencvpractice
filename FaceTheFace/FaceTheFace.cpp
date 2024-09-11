#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

bool key_check(int& no)
{
	int key = waitKeyEx(0);
	if (key == 2621440)			no++;
	else if (key == 2490368)	no--;
	else if (key == 32 || key == 27) return false;

	return true;
}

void load_cascade(CascadeClassifier& cascade, string fname)
{
	string path = "C:/opencv/sources/data/haarcascades/";
	string full_name = path + fname;
	//\ /
	CV_Assert(cascade.load(full_name));
}

Mat preprocessing(Mat image)
{
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);
	equalizeHist(gray, gray);

	return gray;
}

Point2d calc_center(Rect obj)
{
	Point2d c = (Point2d)obj.size() / 2.0;
	Point2d center = (Point2d)obj.tl() + c;

	return center;
}

Mat calc_rotMap(Point2d face_center, vector<Point2f> pt) // ȸ��
{
	Point2d delta = (pt[0].x > pt[1].x) ? pt[0] - pt[1] : pt[1] - pt[0];
	double angle = fastAtan2(delta.y, delta.x);

	Mat rot_mat = getRotationMatrix2D(face_center, angle, 1);
	return rot_mat;
}

Mat correct_image(Mat image, Mat rot_mat, vector<Point2f>& eyes_center) // ȸ��
{
	Mat correct_img;
	warpAffine(image, correct_img, rot_mat, image.size(), INTER_CUBIC);

	for (int i = 0; i < eyes_center.size(); i++)
	{
		Point3d coord(eyes_center[i].x, eyes_center[i].y, 1);
		Mat dst = rot_mat * (Mat)coord;
		eyes_center[i] = (Point2f)dst;
	}
	return correct_img;
}

Rect detect_lip(Point2d face_center, Rect face) {

	Point2d lip_center = face_center + Point2d(0, face.height * 0.3); //�Լ��߽�
	Point2d gap_size(face.width * 0.18, face.height * 0.1); // ������ 18%, ���̿� 10%

	Point lip_start = lip_center - gap_size; //�Լ��� �»��
	Point lip_end = lip_center + gap_size; // �Լ��� ���ϴ�

	return Rect(lip_start, lip_end); //�Լ����� return

}

void detect_hair(Point2d face_center, Rect face, vector<Rect>& hair_rect)
{
	Point2d h_gap(face.width * 0.45, face.height * 0.65);
	Point2d pt1 = face_center - h_gap; // ���� ��� �����̵�
	Point2d pt2 = face_center + h_gap; // ������ �ϴ� �����̵�
	Rect hair(pt1, pt2); // ��ü�Ӹ�����

	Size size(hair.width, hair.height * 0.40); // ���Ӹ� �� �͹ظӸ� ũ��
	Rect hair1(hair.tl(), size); // ���Ӹ� ����
	Rect hair2(hair.br() - (Point)size, size); // �͹ظӸ� ����

	hair_rect.push_back(hair1); // Rect�� ���Ϳ� ����
	hair_rect.push_back(hair2);
	hair_rect.push_back(hair); // ��ü�Ӹ� ������ �������� ����
}

void calc_Histo(const Mat& img, Mat& hist, Vec3i bins, Vec3f range, Mat mask)
{
	int dims = img.channels();
	int channels[] = { 0,1,2 };
	int histSize[] = { bins[0],bins[1],bins[2] };

	float range1[] = { 0, range[0] };
	float range2[] = { 0, range[1] };
	float range3[] = { 0, range[2] };
	const float* ranges[] = { range1,range2,range3 };

	calcHist(&img, 1, channels, mask, hist, dims, histSize, ranges);
}

void calc_histos(const Mat& correct_img, vector<Rect> sub_obj, Mat hists[4], Mat masks[4])
{
	Vec3i bins(64, 64, 64); // ������׷� ��� ����
	Vec3f ranges(256, 256, 256); // �� ä���� ȭ�ҹ���

	for (int i = 0; i < (int)sub_obj.size(); i++)
	{
		Mat sub = correct_img(sub_obj[i]); // ���� ���꿵��
		calc_Histo(sub, hists[i], bins, ranges, masks[i]); // ������׷� ���
		//imshow(format("masks[%d]",i), masks[i]); //����ũ �������
		//waitKey();
	}
}

void draw_ellipse(Mat& image, Rect2d obj, Scalar color, int thickness, float ratio)
{
	Point2d center = obj.tl() + (Point2d)obj.size() * 0.5; // Ÿ�� �߽�
	Size2d size = (Size2d)obj.size() * 0.45; // Ÿ��ũ��
	ellipse(image, center, size, 0, 0, 360, color, thickness);
}

void make_masks(vector<Rect> sub_obj, Size org_size, Mat mask[4])
{
	Mat base_mask(org_size, CV_8U, Scalar(0)); // �Է�ũ��� ����ũ�� 0���� ����
	draw_ellipse(base_mask, sub_obj[2], Scalar(255), -1, 0.45f);

	mask[0] = ~base_mask(sub_obj[0]); //���Ӹ� ����ũ
	mask[1] = ~base_mask(sub_obj[1]); // �͹ظӸ� ����ũ

	draw_ellipse(base_mask, sub_obj[3], Scalar(0), -1, 0.45f);
	mask[3] = ~base_mask(sub_obj[3]); // �Լ�����ũ
	mask[2] = base_mask(sub_obj[2]); // �󱼸���ũ
}

void classify(Mat& image, Mat hists[4], int no)
{
	double criteria1 = compareHist(hists[0], hists[1], HISTCMP_CORREL);
	double criteria2 = compareHist(hists[2], hists[3], HISTCMP_CORREL);
	//���絵 ��
	double tmp = (criteria1 > 0.2) ? 0.2 : 0.4;
	int value = (criteria2 > tmp) ? 0 : 1;
	string text = (value) ? "Man" : "Woman";
	text = format("%02d.jpg: ", no) + text;

	//�з���� ���� ���

	int font = FONT_HERSHEY_TRIPLEX;
	putText(image, text, Point(12, 31), font, 0.7, Scalar(0, 0, 0), 2); // �׸���
	putText(image, text, Point(10, 30), font, 0.7, Scalar(0, 255, 0), 1); // �׸���

	cout << text << format(" - ���絵 [�Ӹ�: %4.2f �Լ�: %4.2f]\n", criteria1, criteria2);

}

void display(Mat& image, Point2d face_center, vector<Point2f> eyes_center, vector<Rect>sub)
{
	circle(image, eyes_center[0], 10, Scalar(0, 255, 0), 2); // �� ǥ��
	circle(image, eyes_center[1], 10, Scalar(0, 255, 0), 2);
	circle(image, face_center, 3, Scalar(0, 0, 255), 2); //�� �߽��� ǥ��

	draw_ellipse(image, sub[2], Scalar(255, 100, 0), 2, 0.45f);// �Լ�Ÿ��
	draw_ellipse(image, sub[3], Scalar(0, 0, 255), 2, 0.45f);// ��Ÿ��

}

void file_face_detect(int select)
{
	CascadeClassifier face_cascade, eyes_cascade;

	load_cascade(face_cascade, "haarcascade_frontalface_alt2.xml");
	load_cascade(eyes_cascade, "haarcascade_eye.xml");

	int no = select;
	do {
		String fname = format("./face/%02d.jpg", no);
		Mat image = imread(fname, IMREAD_COLOR);

		if (image.empty())
		{
			cout << format("%02d������ �����ϴ�.\n", no++);
			continue;
		}

		//CV_Assert(image.data);
		Mat gray = image;


		vector<Rect> faces, eyes, sub_obj;
		vector<Point2f> eyes_center;
		face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100, 100)); // face // problem



		if (faces.size() > 0)
		{
			eyes_cascade.detectMultiScale(gray(faces[0]), eyes, 1.15, 7, 0, Size(25, 20));

			if (eyes.size() == 2)
			{
				eyes_center.push_back(calc_center(eyes[0] + faces[0].tl()));
				eyes_center.push_back(calc_center(eyes[1] + faces[0].tl()));

				Point2d face_center = calc_center(faces[0]);
				Mat rot_mat = calc_rotMap(face_center, eyes_center);
				Mat correct_img = correct_image(image, rot_mat, eyes_center);

				detect_hair(face_center, faces[0], sub_obj);
				sub_obj.push_back(detect_lip(face_center, faces[0]));

				Mat masks[4], hists[4];
				make_masks(sub_obj, correct_img.size(), masks);
				calc_histos(correct_img, sub_obj, hists, masks);

				classify(correct_img, hists, no);
				display(correct_img, face_center, eyes_center, sub_obj);
				imshow("correct_img", correct_img);

				imshow("usb_obj[0]", correct_img(sub_obj[0]));
				imshow("usb_obj[1]", correct_img(sub_obj[1]));
				imshow("usb_obj[2]", correct_img(sub_obj[2]));

				rectangle(correct_img, sub_obj[2], Scalar(255, 0, 0), 2);
				rectangle(correct_img, sub_obj[3], Scalar(0, 255, 0), 2);
				imshow("correct_img", correct_img);

			}
		}
	} while (key_check(no));
}



void face_detect(Mat frame)
{
	CascadeClassifier face_cascade, eyes_cascade;

	load_cascade(face_cascade, "haarcascade_frontalface_alt2.xml");
	load_cascade(eyes_cascade, "haarcascade_eye.xml");

	int no = 0;
	//do {
	Mat image = preprocessing(frame);

	Mat gray = preprocessing(frame);


	vector<Rect> faces, eyes, sub_obj;
	vector<Point2f> eyes_center;
	face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100, 100)); // face // problem


	if (faces.size() > 0)
	{
		eyes_cascade.detectMultiScale(gray(faces[0]), eyes, 1.15, 7, 0, Size(25, 20));
		//imshow("s", gray);

		if (eyes.size() == 2)
		{
			eyes_center.push_back(calc_center(eyes[0] + faces[0].tl())); // �� �߽���
			eyes_center.push_back(calc_center(eyes[1] + faces[0].tl()));

			Point2d face_center = calc_center(faces[0]); // �� �߽���

			//Mat rot_mat = calc_rotMap(face_center, eyes_center); // ���� ����
			//Mat correct_img = correct_image(image, rot_mat, eyes_center);

			detect_hair(face_center, faces[0], sub_obj); // �Ӹ������� ����
			sub_obj.push_back(detect_lip(face_center, faces[0])); // �Լ����� ����

			Mat masks[4], hists[4];
			make_masks(sub_obj, frame.size(), masks); // 4�� ����ũ ����
			calc_histos(frame, sub_obj, hists, masks); // 4�� ������׷�

			classify(frame, hists, no); // �����з� �� ǥ��
			display(frame, face_center, eyes_center, sub_obj); // ��, ��ǥ��
			//imshow("faces", faces);

			imshow("usb_obj[0]", frame(sub_obj[0]));
			imshow("usb_obj[1]", frame(sub_obj[1]));
			imshow("usb_obj[2]", frame(sub_obj[2]));

			rectangle(frame, sub_obj[2], Scalar(255, 0, 0), 2);
			rectangle(frame, sub_obj[3], Scalar(0, 255, 0), 2);
			imshow("ī�޶�", frame);

		}

		rectangle(frame, faces[0], Scalar(255, 0, 0), 2);

	}
	//}while(key_check(no));

}

void camera_face_dectect()
{
	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cout << "ī�޶� ����Ǿ� ���� �ʽ��ϴ�." << '\n';
		exit(1);
	}
	else
	{
		while (1)
		{
			Mat frame;
			capture.read(frame);
			face_detect(frame);

			imshow("ī�޶� ����!", frame);
			if (waitKey(100) >= 0)
				break;
		}
	}
}

int main(void)
{
	int select = 0;
	cout << "��带 �Է��Ͻʽÿ�.(1�� ī�޶� �Է¸�� 0�� ���� �Է¸��		�׿� ����)" << '\n';
	cin >> select;
	if (select == 0)
	{
		cout << "���ϴ� ������ ������ �ֽʽÿ�. (0 ~ 62 �� �ϳ��� �Է��Ͻø� �˴ϴ�.)" << endl;
		cin >> select;
		file_face_detect(select);
	}
	else if (select == 1)
	{
		camera_face_dectect();
	}
	return 0;
}