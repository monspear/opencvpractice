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

Mat calc_rotMap(Point2d face_center, vector<Point2f> pt) // 회전
{
	Point2d delta = (pt[0].x > pt[1].x) ? pt[0] - pt[1] : pt[1] - pt[0];
	double angle = fastAtan2(delta.y, delta.x);

	Mat rot_mat = getRotationMatrix2D(face_center, angle, 1);
	return rot_mat;
}

Mat correct_image(Mat image, Mat rot_mat, vector<Point2f>& eyes_center) // 회전
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

	Point2d lip_center = face_center + Point2d(0, face.height * 0.3); //입술중심
	Point2d gap_size(face.width * 0.18, face.height * 0.1); // 얼굴폭의 18%, 높이에 10%

	Point lip_start = lip_center - gap_size; //입술의 좌상단
	Point lip_end = lip_center + gap_size; // 입술의 우하단

	return Rect(lip_start, lip_end); //입술범위 return

}

void detect_hair(Point2d face_center, Rect face, vector<Rect>& hair_rect)
{
	Point2d h_gap(face.width * 0.45, face.height * 0.65);
	Point2d pt1 = face_center - h_gap; // 왼쪽 상단 평행이동
	Point2d pt2 = face_center + h_gap; // 오른쪽 하단 평행이동
	Rect hair(pt1, pt2); // 전체머리영역

	Size size(hair.width, hair.height * 0.40); // 윗머리 및 귀밑머리 크기
	Rect hair1(hair.tl(), size); // 윗머리 영역
	Rect hair2(hair.br() - (Point)size, size); // 귀밑머리 영역

	hair_rect.push_back(hair1); // Rect형 벡터에 저장
	hair_rect.push_back(hair2);
	hair_rect.push_back(hair); // 전체머리 영역을 마지막에 저장
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
	Vec3i bins(64, 64, 64); // 히스토그램 계급 개수
	Vec3f ranges(256, 256, 256); // 각 채널의 화소범위

	for (int i = 0; i < (int)sub_obj.size(); i++)
	{
		Mat sub = correct_img(sub_obj[i]); // 검출 서브영상
		calc_Histo(sub, hists[i], bins, ranges, masks[i]); // 히스토그램 계산
		//imshow(format("masks[%d]",i), masks[i]); //마스크 영상출력
		//waitKey();
	}
}

void draw_ellipse(Mat& image, Rect2d obj, Scalar color, int thickness, float ratio)
{
	Point2d center = obj.tl() + (Point2d)obj.size() * 0.5; // 타원 중심
	Size2d size = (Size2d)obj.size() * 0.45; // 타원크기
	ellipse(image, center, size, 0, 0, 360, color, thickness);
}

void make_masks(vector<Rect> sub_obj, Size org_size, Mat mask[4])
{
	Mat base_mask(org_size, CV_8U, Scalar(0)); // 입력크기와 같은크기 0값을 가짐
	draw_ellipse(base_mask, sub_obj[2], Scalar(255), -1, 0.45f);

	mask[0] = ~base_mask(sub_obj[0]); //윗머리 마스크
	mask[1] = ~base_mask(sub_obj[1]); // 귀밑머리 마스크

	draw_ellipse(base_mask, sub_obj[3], Scalar(0), -1, 0.45f);
	mask[3] = ~base_mask(sub_obj[3]); // 입술마스크
	mask[2] = base_mask(sub_obj[2]); // 얼굴마스크
}

void classify(Mat& image, Mat hists[4], int no)
{
	double criteria1 = compareHist(hists[0], hists[1], HISTCMP_CORREL);
	double criteria2 = compareHist(hists[2], hists[3], HISTCMP_CORREL);
	//유사도 비교
	double tmp = (criteria1 > 0.2) ? 0.2 : 0.4;
	int value = (criteria2 > tmp) ? 0 : 1;
	string text = (value) ? "Man" : "Woman";
	text = format("%02d.jpg: ", no) + text;

	//분류결과 영상에 출력

	int font = FONT_HERSHEY_TRIPLEX;
	putText(image, text, Point(12, 31), font, 0.7, Scalar(0, 0, 0), 2); // 그림자
	putText(image, text, Point(10, 30), font, 0.7, Scalar(0, 255, 0), 1); // 그림자

	cout << text << format(" - 유사도 [머리: %4.2f 입술: %4.2f]\n", criteria1, criteria2);

}

void display(Mat& image, Point2d face_center, vector<Point2f> eyes_center, vector<Rect>sub)
{
	circle(image, eyes_center[0], 10, Scalar(0, 255, 0), 2); // 눈 표시
	circle(image, eyes_center[1], 10, Scalar(0, 255, 0), 2);
	circle(image, face_center, 3, Scalar(0, 0, 255), 2); //얼굴 중심정 표시

	draw_ellipse(image, sub[2], Scalar(255, 100, 0), 2, 0.45f);// 입술타원
	draw_ellipse(image, sub[3], Scalar(0, 0, 255), 2, 0.45f);// 얼굴타원

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
			cout << format("%02d영상이 없습니다.\n", no++);
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
			eyes_center.push_back(calc_center(eyes[0] + faces[0].tl())); // 눈 중심점
			eyes_center.push_back(calc_center(eyes[1] + faces[0].tl()));

			Point2d face_center = calc_center(faces[0]); // 얼굴 중심점

			//Mat rot_mat = calc_rotMap(face_center, eyes_center); // 기울기 각도
			//Mat correct_img = correct_image(image, rot_mat, eyes_center);

			detect_hair(face_center, faces[0], sub_obj); // 머리영역들 검출
			sub_obj.push_back(detect_lip(face_center, faces[0])); // 입술영역 검출

			Mat masks[4], hists[4];
			make_masks(sub_obj, frame.size(), masks); // 4개 마스크 생성
			calc_histos(frame, sub_obj, hists, masks); // 4개 히스토그램

			classify(frame, hists, no); // 성별분류 및 표시
			display(frame, face_center, eyes_center, sub_obj); // 얼굴, 눈표시
			//imshow("faces", faces);

			imshow("usb_obj[0]", frame(sub_obj[0]));
			imshow("usb_obj[1]", frame(sub_obj[1]));
			imshow("usb_obj[2]", frame(sub_obj[2]));

			rectangle(frame, sub_obj[2], Scalar(255, 0, 0), 2);
			rectangle(frame, sub_obj[3], Scalar(0, 255, 0), 2);
			imshow("카메라", frame);

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
		cout << "카메라가 연결되어 있지 않습니다." << '\n';
		exit(1);
	}
	else
	{
		while (1)
		{
			Mat frame;
			capture.read(frame);
			face_detect(frame);

			imshow("카메라 영상!", frame);
			if (waitKey(100) >= 0)
				break;
		}
	}
}

int main(void)
{
	int select = 0;
	cout << "모드를 입력하십시오.(1번 카메라 입력모드 0번 파일 입력모드		그외 종료)" << '\n';
	cin >> select;
	if (select == 0)
	{
		cout << "원하는 파일을 선택해 주십시오. (0 ~ 62 에 하나를 입력하시면 됩니다.)" << endl;
		cin >> select;
		file_face_detect(select);
	}
	else if (select == 1)
	{
		camera_face_dectect();
	}
	return 0;
}