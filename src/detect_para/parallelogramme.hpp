array<Mat, 2> calcule_bdf(Mat m)
{
	Size taille = m.size();
	int tailleX = taille.width;
	int tailleY = taille.height;
	Mat resX = Mat::zeros(taille, CV_8SC1);
	Mat resY = Mat::zeros(taille, CV_8SC1);

	for(int x = 0; x < tailleX; ++x)
	{
		for(int y = 0; y < tailleY; ++y)
		{
			for(int dx = -3; dx <= 3; ++dx)
			{
				for(int dy = -3; dy <= 3; ++dy)
				{
					if(0 <= x+dx < tailleX && 0 <= y+dy < tailleY)
					{
						resX.at<uint8_t>(y,x) += floor(64.0*dx/((dx*dx + dy*dy)*sqrt(dx*dx + dy*dy)));
						resY.at<uint8_t>(y,x) += floor(64.0*dy/((dx*dx + dy*dy)*sqrt(dx*dx + dy*dy)));
					}
				}
			}
		}
	}
	array<Mat, 2> res = {resX, resY};
	return res;
}
