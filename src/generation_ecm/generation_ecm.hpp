#pragma once

Mat calcul_R(vector<Mat> ensembles_O, vector<Mat> ensembles_S, int t, int T, int nb_orientations)
{
	// Calcule R_t à partir du vecteur des valeurs de O à chaque instant t.
	// R_t est l'ensemble des points ayant une orientation fixe pendant T images.
	// L'image renvoyée contient des entiers valant 0 si le pixel n'est pas dans R, et un entier sinon
	Size taille = ensembles_O.at(0).size();
	int taille_x = taille.width;
	int taille_y = taille.height;

	Mat res = Mat(taille, CV_8UC1, 1);
	
	// Puis on vérifie la stabilité des orientations
	for(int x = 0; x < taille_x; ++x) 
	for(int y = 0; y < taille_y; ++y) 
	for(int i = t-T; i <= t; ++i)
	{
		if(ensembles_S.at(i).at<uint8_t>(y,x) > 4 && 
			abs((ensembles_O.at(i).at<uint8_t>(y,x)-ensembles_O.at(t-T).at<uint8_t>(y,x))%nb_orientations) > 1)
		{
			res.at<uint8_t>(y,x) = 0;
		}
	}
	imshow("R", res*255);
	attend_q();
	return res;
}

Mat calcul_D(vector<Mat> ensembles_O, vector<Mat> ensembles_S, int t, int T, int nb_orientations, uint8_t seuil)
{
	// Calcule l'ensemble D_t des pixels pour lesquels un mouvement a été detecté à l'instant t
	// Cela implique soit que son orientation diffère de R_t, soit qu'il n'est pas dans R_t
	// Si le pixel vaut 0, aucun mouvement n'a été détecté.
	// Sinon, le pixel vaut 1
	Size taille = ensembles_O.at(0).size();
	int taille_x = taille.width;
	int taille_y = taille.height;
	
	Mat res = Mat::zeros(taille, CV_8UC1);
	Mat R = calcul_R(ensembles_O, ensembles_S, t, T, nb_orientations);
	Mat disp_R;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			if(R.at<uint8_t>(y,x) == 0 && ensembles_S.at(t).at<uint8_t>(y,x) > seuil && abs(ensembles_O.at(t-1).at<uint8_t>(y,x) - ensembles_O.at(t).at<uint8_t>(y,x)) > 2 )
			{
				res.at<uint8_t>(y,x) = 1;
			}
		}
	}
	//imshow("D", res*255);
	return res;
}
