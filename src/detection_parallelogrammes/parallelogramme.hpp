#pragma once

array<Mat, 2> calcule_bdf(Mat m)
{
	Size taille = m.size();
	int tailleX = taille.width;
	int tailleY = taille.height;
	Mat resX = Mat::zeros(taille, CV_16SC1);
	Mat resY = Mat::zeros(taille, CV_16SC1);
	float isr; // inv square root

	for(int x = 0; x < tailleX; ++x)
	{
		for(int y = 0; y < tailleY; ++y)
		{
			for(int dx = -6; dx <= 6; ++dx)
			{
				for(int dy = -6; dy <= 6; ++dy)
				{
					if(0 <= x+dx && x+dx < tailleX && 0 <= y+dy && y+dy < tailleY)
					{
						isr = sqrt(float(dx*dx + dy*dy));
						resX.at<int16_t>(y+dy,x+dx) += floor(dx * isr*isr*isr * m.at<uint8_t>(y,x));
						resY.at<int16_t>(y+dy,x+dx) += floor(dy * isr*isr*isr * m.at<uint8_t>(y,x));
					}
				}
			}
		}
	}
	array<Mat, 2> res = {resX, resY};
	return res;
}
