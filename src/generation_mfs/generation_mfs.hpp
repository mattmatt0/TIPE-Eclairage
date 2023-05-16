#include "detection_contours/detection_contours.hpp"

Mat calcule_R(vector<Mat> ensembles_O, int t, int T, int nb_orientations)
{
	// Calcule R_t à partir du vecteur des valeurs de O à chaque instant t.
	// R_t est l'ensemble des points ayant une orientation fixe pendant T images.
	// L'image renvoyée contient des entiers valant 0 si le pixel n'est pas dans R, et un entier représentant l'orientation du pixel sinon.
	int taille_x = ensembles_O.at(0).size().height;
	int taille_y = ensembles_O.at(0).size().width;

	Mat res = ensembles_O.at(0).clone();
	
	cout << "Vérification de la stabilité..." << endl;
	// Puis on vérifie la stabilité des orientations
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			for(int i = t+1; i < t+T; ++i)
			{
				if(res.at<uint8_t>(x,y) != ensembles_O.at(i).at<uint8_t>(x,y))
				{
					res.at<uint8_t>(x,y) = 255;
					break;
				}
			}
		}
	}
	cout << "Terminé !" << endl;
	return res;
}

Mat calcule_D(vector<Mat> images, int t, int T, int nb_orientations)
{
	// Calcule l'ensemble D_t des pixels pour lesquels un mouvement a été detecté à l'instant t
	// Cela implique soit que son orientation diffère de R_t, soit qu'il n'est pas dans R_t
	// Si le pixel vaut 0, aucun mouvement n'a été détecté.
	// Sinon, le pixel vaut 1
	int seuil = 360 / (nb_orientations+1);
	int taille_x = images.at(0).size().height;
	int taille_y = images.at(0).size().width;
	
	Mat res = Mat::zeros(images.at(0).size(), CV_8UC1);
	Mat R = calcule_R(images, t, T, nb_orientations);
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			if(R.at<uint8_t>(x,y) == 255 || R.at<uint8_t>(x,y) != images.at(t+T).at<uint8_t>(x,y))
			{
				res.at<uint8_t>(x,y) = images.at(t+T).at<uint8_t>(x,y);
			}
		}
	}
	return res;
}
