#include "detection_contours/detection_contours.hpp"

Mat calcule_R(vector<Mat> ensembles_O, int t, int T, int nb_orientations)
{
	// Calcule R_t à partir du vecteur des valeurs de O à chaque instant t.
	// R_t est l'ensemble des points ayant une orientation fixe pendant T images.
	// L'image renvoyée contient des entiers valant 0 si le pixel n'est pas dans R, et un entier représentant l'orientation du pixel sinon.
	int seuil = 360 / (nb_orientations+1);
	int taille_x = ensembles_O.at(0).size().height;
	int taille_y = ensembles_O.at(0).size().width;
	// Création de la première image à laquelle on va comparer les images suivantes.
	// On va arrondir les valeurs des orientations comme il faut, dans un demi-tour
	Mat res = Mat(ensembles_O.at(0).size(), CV_8UC1);
	cout << "Conversion de l'image en valeurs entières..." << endl;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			res.at<uint8_t>(x,y) = floor(floor(ensembles_O.at(t).at<float>(x,y) / 360 * nb_orientations) * seuil);
		}
	}
	return res;
	
	cout << "Vérification de la stabilité..." << endl;
	// Puis on vérifie la stabilité des orientations
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			cout << "En x =" << x << " et y = " << y << endl;
			for(int i = t+1; i < t+T; ++i)
			{
				if(res.at<int>(x,y) > ensembles_O.at(i).at<float>(x,y) + seuil ||
					res.at<int>(x,y) < ensembles_O.at(i).at<float>(x,y) - seuil)
				{
					res.at<int>(x,y) = 0;
					break;
				}
			}
		}
	}
	cout << "Terminé !" << endl;
	return res;
}

/*
Mat calcule_D(vector<Mat> images, int t,  int nb_orientations)
{
	// Calcule l'ensemble D_t des pixels pour lesquels un mouvement a été detecté à l'instant t
	// Cela implique soit que son orientation diffère de R_t, soit qu'il n'est pas dans R_t
	// Si le pixel vaut 0, aucun mouvement n'a été détecté.
	// Sinon, le pixel vaut 1
}
*/
