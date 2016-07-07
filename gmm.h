#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include "opencv2/imgproc.hpp"
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include <math.h>
#include <algorithm>
#include <functional>
#include <windows.h>

//parameters for gmm 
#define  VAR_INIT	256.0
#define  pi			3.1415926
#define  alpha		0.005
#define	 MODE_MAX	5
#define  bg_ratio	0.90
#define  frame_s    1080

//block increment
#define	 b_i		5

class gmm_c
{
public:
	int		mode_cnt = 0;
	bool	gmm_bgs(gmm_c *p, cv::Vec3b n_p);
	void	modes_print();


	double	m_b[MODE_MAX];				//bgr pixel value
	double	m_g[MODE_MAX];
	double	m_r[MODE_MAX];
	double	var[MODE_MAX];
	double	weight[MODE_MAX];
	double	rho = 0.05;
};

//Just for test
struct p_vec
{
	double	p_v;
	int		r;
	int		c;
};


bool st_comp(p_vec a, p_vec b);
void bg_prc(cv::Mat *in);

