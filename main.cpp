#include "gmm.h"
using namespace cv;
using namespace std;

bool	 block_flag = 1;

vector<p_vec> block_sort(cv::Mat *in, int r, int c)
{
	int i = 0, t;
	//int **ind = new int*[b_i];
	//for (t = 0; t < b_i; t++)
	//	ind[t] = new int[b_i];

	vector<p_vec> sorted_block(b_i * b_i);
	for (int m = r; m < r + b_i; m = m + 1)
	{
		for (int n = c; n < c + b_i; n = n + 1)
		{
			//if(i < 16)
			//{ 
			sorted_block[i].p_v = (double)in->at<uchar>(m, n);
			sorted_block[i].r = m;
			sorted_block[i].c = n;
			i++;
			//}
		}
	}
	sort(sorted_block.begin(), sorted_block.end(), st_comp);
	/*
	for (t = 0; t < b_i*b_i; t++)
	{
	ind[0][t] = sorted_block[i].r;
	ind[1][t] = sorted_block[i].c;
	}
	*/
	return sorted_block;

}

int main()
{
	char *s = "canoe.avi";
	VideoCapture cap(s);
	if (!cap.isOpened())
	{
		cout << "!!! Failed to open file: " << "Video_008.avi" << std::endl;
		return -1;
	}
	Mat frame;

	if (!cap.read(frame))
		return -1;
	//imshow("window", frame);
	waitKey(30);

	bool flag;
	Vec3b intensity;
	int r, c;
	vector<p_vec> tmp(b_i * b_i);
	int i = 1;
	//char s[13];
	char n2s[7];
	int  f_cnt = 0;
	int  b_cnt = 0;
	Mat img0, gray_img,fgmask, fgimg;
	img0 = frame;
	/*
	strcpy(s, "in");
	sprintf_s(n2s, "%06d", i);
	strcat_s(s, n2s);
	strcat_s(s, ".jpg");
	img0 = imread(s, CV_LOAD_IMAGE_COLOR);
	*/
	if (!img0.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	const int row_n = img0.rows;
	const int col_n = img0.cols;
	Mat img(row_n, col_n, CV_8UC1);
	gmm_c **p1 = new gmm_c*[row_n];												//create gmm buffer
	for(i = 0; i < row_n; i++)
		p1[i] = new gmm_c[col_n];
	for (i = 1; i < frame_s; i++)
	{
		cap.read(frame);
		f_cnt = 0;
		b_cnt = 0;
		DWORD start = GetTickCount();
		/*
		strcpy(s, "in");
		sprintf_s(n2s, "%06d", i);
		strcat_s(s, n2s);
		strcat_s(s, ".jpg");
		img0 = imread(s, CV_LOAD_IMAGE_COLOR);   // Read the file
		*/
		if (!img0.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << endl;
			return -1;
		}
		if(1 == block_flag)
			cvtColor(img0, gray_img, CV_BGR2GRAY);
		if (1 == block_flag)
		{
			for (r = 0; r < img0.rows; r = r + b_i)
			{
				for (c = 0; c < img0.cols; c = c + b_i)
				{
				
					tmp = block_sort(&gray_img, r, c);
					for (int b_cnt = 0; b_cnt < b_i * b_i; b_cnt++)
					{
						intensity = img0.at<Vec3b>(tmp[b_cnt].r, tmp[b_cnt].c);
						flag = p1[r + b_cnt / b_i][c + b_cnt % b_i].gmm_bgs(&p1[r + b_cnt / b_i][c + b_cnt % b_i], intensity);
						if (0 == flag)											//0 == flag means foreground
						{
							img.at<uchar>(tmp[b_cnt].r, tmp[b_cnt].c) = 255;
							//f_cnt++;
						}
						if (1 == flag)											//background
						{
							img.at<uchar>(tmp[b_cnt].r, tmp[b_cnt].c) = 0;
							//b_cnt++;
						}
					}
				}
			}
		}
		else
		{
			for (r = 0; r < img0.rows; r++)
			{
				for (c = 0; c < img0.cols; c++)
				{
					intensity = img0.at<Vec3b>(r, c);
					flag = p1[r][c].gmm_bgs(&p1[r][c], intensity);
					if (0 == flag)											//0 == flag means foreground
					{
						img.at<uchar>(r, c) = 255;
						//f_cnt++;
					}
					if (1 == flag)											//background
					{
						img.at<uchar>(r, c) = 0;
						//b_cnt++;
					}
				}
			}
		}
		DWORD end = GetTickCount();
		if (247 == i)
		{
			for (int m = 0; m < img.rows; m++)
				for (int n = 0; n < img.cols; n++)
				{ 
					if (255 == img.at<uchar>(m, n))
						f_cnt++;
				}

			cout << f_cnt<<endl;
		}
		bg_prc(&img0);
		cout << "duration = " << (double)(end - start)  << "sec.\n";
		imshow("original",img0);						//show original frame
		imshow("foreground", img);						//show binary foreground frame
		waitKey(30);
		/*Just for test
		resize(img0, img, Size(640, 640 * img0.rows / img0.cols), INTER_LINEAR);
		if (fgimg.empty())
			fgimg.create(img.size(), img.type());
		bg_model->apply(img, fgmask, update_bg_model ? -1 : 0);
		fgimg = Scalar::all(0);
		img.copyTo(fgimg, fgmask);
		Mat bgimg;
		bg_model->getBackgroundImage(bgimg);
		
		imshow("image", img);
		imshow("foreground mask", fgmask);
		imshow("foreground image", fgimg);
		waitKey(30);
		*/
	}

	namedWindow(s, WINDOW_AUTOSIZE);// Create a window for display.
	imshow(s, img);
	//
	int mfgcnt = 0;
	int mbgcnt = 0;
	int mzcnt = 0;
	Mat gt = imread("gt000703.png",1);
	if (!gt.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
		for (i = 0; i < row_n; i++)
			delete[] p1[i];

		delete p1;
		cout << "delete Success" << endl;;
		return -1;
	}
	namedWindow("Ground Truth", WINDOW_AUTOSIZE);
	imshow("Ground Truth", gt);

	for (r = 0; r < img.rows; r++)
	{
		for (c = 0; c < img.cols; c++)
		{
			if (255 == img.at<uchar>(r, c) && 0 == gt.at<Vec3b>(r, c).val[0])
				mfgcnt++;
			if (0 == img.at<uchar>(r, c) && 255 == gt.at<Vec3b>(r, c).val[0])
				mbgcnt++;
		}
	}

	cout << "mfg: "<<mfgcnt<<" mbgcnt: "<< mbgcnt << endl;
	cout << "Press any key to quit" << endl;
	char key;
	waitKey(9000);
	//cin >> key;
	//if('\0' != key)
	//{
	//	cout << "Quit now" << endl;
	//}
	for (i = 0; i < row_n; i++)
		delete[] p1[i];

	delete p1;
	cout << "delete Success" << endl;
	return 0;
}
