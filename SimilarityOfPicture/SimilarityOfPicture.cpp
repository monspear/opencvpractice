#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void calc_Histo(const Mat& img, Mat& hist, Vec3i bins, Vec3i range, int _dims)
{
	int dims = (_dims <= 0) ? img.channels() : _dims;
	int channels[] = { 0,1,2 };
	int histSize[] = { bins[0],bins[1],bins[2] };

	float range1[] = { 0,range[0] };
	float range2[] = { 0,range[1] };
	float range3[] = { 0,range[2] };

	const float* ranges[] = { range1,range2,range3 };

	calcHist(&img, 1, channels, Mat(), hist, dims, histSize, ranges);
	normalize(hist, hist, 0, 1, NORM_MINMAX);
}
vector<Mat> load_histo(Vec3i bins, Vec3i ranges, int image_num)
{
	vector<Mat> DB_hists;
	for (int i = 0; i < image_num; i++)
	{
		string fname = format("./image/img_%02d.jpg", i); // 100개의 이미지 파일 이름
		Mat hsv, hist, img = imread(fname, IMREAD_COLOR); // 파일 이름을 읽어서 img에 저장
		if (img.empty()) continue; // img 비어있는 경우 중단

		cvtColor(img, hsv, COLOR_BGR2HSV); // hsv에 이미지를 h,s,v 값을 가지도록 변환
		calc_Histo(hsv, hist, bins, ranges, 2); // 2차원 히스토그램 계산 hist에 저장
		DB_hists.push_back(hist); // 다음행에  히스토그램 하나씩 저장
	}
	cout << format("^d 개의 파일 로드 및 히스토그램 계산 완료되었습니다.", DB_hists.size());

	return DB_hists; // DB_hist( 100 * (30 * 42) )배열모양
}

Mat calc_similarity(Mat query_hist, vector<Mat>& DB_hists)
{
	Mat DB_similaritys;
	for (int i = 0; i < (int)DB_hists.size(); i++)
	{
		double compare = compareHist(query_hist, DB_hists[i], HISTCMP_CORREL);
		// 기준영상과 DB영상의 히스토그램 비교, 유사도 계산

		DB_similaritys.push_back(compare); // 계산한 값을 저장
	}
	return DB_similaritys;
}

void select_view(double sinc, Mat DB_similaritys)
{
	Mat m_idx, sorted_sim;
	int flag = SORT_EVERY_COLUMN + SORT_DESCENDING; // 행단위 + 내림차순
	cv::sort(DB_similaritys, sorted_sim, flag); // 행렬 원소값 정렬
	sortIdx(DB_similaritys, m_idx, flag); // 행렬원소의 원본 인덱스 반환

	for (int i = 0; i < (int)sorted_sim.total(); i++)
	{
		double sim = sorted_sim.at<double>(i); // 유사도 정렬 원소값.

		if (sim > sinc) // 입력받은 기준 보다 유사도가 높은경우
		{
			int idx = m_idx.at<int>(i);
			string fname = format("./image/img_%02d.jpg", idx);
			Mat img = imread(fname, 1);

			string title = format("img_%03d - %5.2f", idx, sim);
			cout << title << '\n';
			imshow(title, img);
		}
		else // 유사도로 정렬되었기에 이후의 값들은 읽을 필요 x
		{
			break;
		}
	}
}

Mat query_img()
{
	while (1)
	{
		int q_no = 74;
		cout << "질의영상 번호를 입력하세요 : " << '\n';
		cin >> q_no;

		String fname = format("./image/img_%02d.jpg", q_no);
		Mat query = imread(fname, IMREAD_COLOR);

		if (query.empty()) // 제대로 입력 안할 경우
			cout << "질의 영상 번호를 다시 입력하십시오.(0 ~ 99)" << '\n';
		else
			return query; // 제대로 입력받으면 리턴
	}
}

Mat draw_histo(Mat hist)
{
	/*
	if (hist.dims != 2) { //이번에 받을 것을 3차원이므로 생략
		cout << "2차원 데이터가 아닙니다." << '\n';
		exit(1);
	}
	*/
	float ratio_value = 512.f;
	float ratio_hue = 180.f / hist.rows;
	float ratio_sat = 256.f / hist.cols;

	Mat graph(hist.size(), CV_32FC3);

	for (int i = 0; i < hist.rows; i++)
	{
		for (int j = 0; j < hist.cols; j++)
		{
			float value = hist.at<float>(i, j) * ratio_value;
			float hue = i * ratio_hue;
			float sat = j * ratio_sat;
			graph.at<Vec3f>(i, j) = Vec3f(hue, sat, value);
		}
	}
	graph.convertTo(graph, CV_8U);
	cvtColor(graph, graph, COLOR_HSV2BGR);
	resize(graph, graph, Size(0, 0), 10, 10, INTER_NEAREST);

	return graph;
}

int main(void)
{
	Vec3i bins(30, 42, 0); // 계급
	Vec3i ranges(180, 256, 0); // 범위
	vector<Mat> DB_hists = load_histo(bins, ranges, 100); // 히스토그램들의 배열을 반환(100 * (30,42))
	Mat query = query_img(); // 입력영상 파일 읽기

	Mat hsv, query_hist;
	cvtColor(query, hsv, COLOR_BGR2HSV); // query 를 hsv 컬러 변환해서 hsv에 저장
	calc_Histo(hsv, query_hist, bins, ranges, 2); // 2차원 히스토그램 계산 (30 * 42)

	Mat hist_img = draw_histo(query_hist); // 2차원 히스토그램그리기 

	Mat DB_simmilaritys = calc_similarity(query_hist, DB_hists); // 입력 영상의 히스토그램과 DB 히스토그램의 유사도 계산

	double sinc;
	cout << "기준 유사도를 입력하십시오(0.0 ~ 1.0) : ";
	cin >> sinc;
	select_view(sinc, DB_simmilaritys); // 입력받은 유사도이상의 영상들을 출력

	imshow("image", query); // 선택한 기준 이미지 출력
	waitKey(0);

	return 0;
}