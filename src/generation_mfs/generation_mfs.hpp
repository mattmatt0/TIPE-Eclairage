#include "detection_contours/detection_contours.hpp"

Mat calcule_R(vector<Mat> ensembles_O, vector<Mat> ensembles_S, int t, int T, int nb_orientations)
{
	// Calcule R_t à partir du vecteur des valeurs de O à chaque instant t.
	// R_t est l'ensemble des points ayant une orientation fixe pendant T images.
	// L'image renvoyée contient des entiers valant 0 si le pixel n'est pas dans R, et un entier sinon
	Size taille = ensembles_O.at(0).size();
	int taille_x = taille.width;
	int taille_y = taille.height;

	Mat res = Mat::zeros(taille, CV_8UC1);
	
	// Puis on vérifie la stabilité des orientations
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			for(int i = t+1; i < t+T; ++i)
			{
				if(abs((int8_t) ensembles_O.at(t).at<uint8_t>(y,x) - (int8_t)ensembles_O.at(i).at<uint8_t>(y,x)) >= 5)
				{
					res.at<uint8_t>(y,x) = 1;
					break;
				}
			}
		}
	}
	return res;
}

Mat calcule_D(vector<Mat> ensembles_O, vector<Mat> ensembles_S, int t, int T, int nb_orientations, uint8_t seuil)
{
	// Calcule l'ensemble D_t des pixels pour lesquels un mouvement a été detecté à l'instant t
	// Cela implique soit que son orientation diffère de R_t, soit qu'il n'est pas dans R_t
	// Si le pixel vaut 0, aucun mouvement n'a été détecté.
	// Sinon, le pixel vaut 1
	Size taille = ensembles_O.at(0).size();
	int taille_x = taille.width;
	int taille_y = taille.height;
	
	Mat res = Mat::zeros(taille, CV_8UC1);
	Mat R = calcule_R(ensembles_O, ensembles_S, t, T, nb_orientations);
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			if(ensembles_S.at(t+T).at<uint8_t>(y,x) >= seuil)
			{
				if(R.at<uint8_t>(y,x) == 0 || abs((int8_t) R.at<uint8_t>(y,x) - (int8_t) ensembles_O.at(t+T).at<uint8_t>(y,x)) > 1)
				{
					res.at<uint8_t>(y,x) = 1;
				}
			}
		}
	}
	return res;
}
