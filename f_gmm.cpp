#include "gmm.h"

using namespace std;
/*
gmm_c::gmm_c(double  b_p, double  g_p, double  r_p)
{
	if (0 == mode_cnt)
	{
		m_b[0] = b_p;					//blue
		m_g[0] = g_p;					//green
		m_r[0] = r_p;					//red
		var[0] = VAR_INIT;
		rho = 0.05;
		mode_cnt = 1;
	}
}
*/

bool gmm_c::gmm_bgs(gmm_c *p, cv::Vec3b n_p)
{
	int		cnt,new_m,s_n;
	bool	flag = 0;
	double  b_p = (double)n_p.val[0];
	double	g_p = (double)n_p.val[1];
	double	r_p = (double)n_p.val[2];
	double	dB, dG, dR, sd, wsum = 0;
	//initial gmm parameter
	if (0 == p->mode_cnt)									//initial flag check 
	{
		p->m_b[0] = b_p;					//blue
		p->m_g[0] = g_p;					//green
		p->m_r[0] = r_p;					//red
		p->var[0] = VAR_INIT;
		p->mode_cnt = 1;								//Initial finished
	}
	else                            
	{
		//Update Model
		for (cnt = 0; cnt < p->mode_cnt; cnt++)
		{
			dB = pow(p->m_b[cnt] - b_p, 2);
			dG = pow(p->m_g[cnt] - g_p, 2);
			dR = pow(p->m_r[cnt] - r_p, 2);

			sd = dB + dG + dR;
			if (sqrt(sd) < 2.5*sqrt(p->var[cnt]))
			{
				flag = 1;
				p->weight[cnt] = (1 - alpha)*p->weight[cnt] + alpha;														////equation 5 in paper
				p->rho = alpha*exp(sd / (-2 * p->var[cnt])) / (pow(2 * pi, 3.0 / 2.0)*sqrt(p->var[cnt]));					//equation 8 in paper
				p->m_b[cnt] = (1 - p->rho)*p->m_b[cnt] + rho*b_p;															//equation 6 in paper
				p->m_g[cnt] = (1 - p->rho)*p->m_g[cnt] + rho*g_p;
				p->m_r[cnt] = (1 - p->rho)*p->m_r[cnt] + rho*r_p;
				p->var[cnt] = (1 - p->rho)*p->var[cnt] + rho*sd;															//equation 7 in paper
			}
			else
			{
				p->weight[cnt] = (1 - alpha)*p->weight[cnt];
			}
			wsum = wsum + p->weight[cnt];
		}
		if (0 == flag)
		{
			if (MODE_MAX == p->mode_cnt)				//when model buffer is full,delete the smallest model
			{	
				vector<pair<double, int>> pairs(MODE_MAX);
				for (int i = 0; i < MODE_MAX; i++)
				{
					pairs[i] = make_pair(p->weight[i] / p->var[i], i);
				}
				sort(pairs.begin(), pairs.end());
				new_m = pairs[0].second;
			}
			else                                          //still have space? mode_cnt increment
			{
				new_m = p->mode_cnt;
				p->mode_cnt++;
			}
																	
			p->m_b[new_m] = b_p;						//create new model
			p->m_g[new_m] = g_p;
			p->m_r[new_m] = r_p;
			p->var[new_m] = VAR_INIT;
			p->weight[new_m] = alpha;

			wsum = 0;

			for (cnt = 0; cnt < p->mode_cnt; cnt++)
			{
				wsum = wsum + p->weight[cnt];
			}

		}
		//
		for (cnt = 0; cnt < p->mode_cnt; cnt++)
		{
			p->weight[cnt] = p->weight[cnt] / wsum;			//normalize the weight
		}

		vector<pair<double, int>> pairs(p->mode_cnt);		//sort the weight
		for (int i = 0; i < p->mode_cnt; i++)
		{
			pairs[i] = make_pair(p->weight[i] / p->var[i], i);
		}
		sort(pairs.begin(), pairs.end(),greater<pair<double,int>>());
		wsum = 0;
		flag = 0;
		for (cnt = 0; cnt < p->mode_cnt; cnt++)				//equation 9 in paper
		{
			s_n = pairs[cnt].second;
			dB = pow(p->m_b[s_n] - b_p, 2);
			dG = pow(p->m_g[s_n] - g_p, 2);
			dR = pow(p->m_r[s_n] - r_p, 2);
			sd = dB + dG + dR;
			if (sqrt(sd) < 2.5 * sqrt(p->var[s_n]))
			{
				if (wsum <= bg_ratio)
				{
					flag = 1;
				}
			}
			wsum = wsum + p->weight[s_n];
		}
	}
	return flag;
}
//Just for test
void gmm_c::modes_print()
{
	/*
	double *p = new double[5 * 5];
	double (*mode_array)[5] = (double(*)[5])p;
	for(int i = 0; i < mode_cnt; i++)
	{
		mode_array[i][0] = m_r[i];
		mode_array[i][1] = m_g[i];
		mode_array[i][2] = m_b[i];
		mode_array[i][3] = var[i];
		mode_array[i][4] = weight[i];
	}
	cout << mode_array[1][1] << endl;
	delete[] p;
	*/
	for (int i = 0; i < mode_cnt; i++)
	{
		cout <<"First:"<< m_r[i]<<' '<<m_g[i]<<' '<<m_b[i]<<' '<<var[i]<<' '<<weight[i]<<endl;
		/*cout << m_r[i] << endl;
		mode_array[i][2] = m_b[i];
		mode_array[i][3] = var[i];
		mode_array[i][4] = weight[i];*/
	}
}

bool st_comp(p_vec a, p_vec b)
{
	return a.p_v < b.p_v;
}

void bg_prc(cv::Mat *in)
{
	int		col = in->cols;
	int		row = in->rows;
	uchar 	up, left, right, down;
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			if (r < 1)
			{
				up = in->at<uchar>(r, c);
			}
			else
			{
				up = in->at<uchar>(r-1, c);
			}
			if (c < 1)
			{
				left = in->at<uchar>(r, c);
			}
			else
			{
				left = in->at<uchar>(r, c-1);
			}
			if (c == col - 1)
			{
				right = in->at<uchar>(r, c);
			}
			else
			{
				right = in->at<uchar>(r, c + 1);
			}
			if (r == row - 1)
			{
				down = in->at<uchar>(r, c);
			}
			else
			{
				down = in->at<uchar>(r+1, c);
			}
			if (255 == up && 255 == left && 255 == right && 255 == down)
			{
				in->at<uchar>(r , c) = 255;
			}
			if (0 == up && 0 == left && 0 == right && 0 == down)
			{
				in->at<uchar>(r, c) = 0;
			}
		}
	}
	
}
