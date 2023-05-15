#include "outils/outils.hpp"
#include "outils/constantes.hpp"


// Coder fonction pour le calcul de Texton Color Space (TCS)
// Définir un nouveau type struct pour renvoyer les 3 canaux

array<Mat, 3> separe_hsv(Mat image_hsv)
{
	array<Mat, 3> canaux;
	split(image_hsv, canaux);
	return canaux;
}

// Tables de valeurs pour accélérer le calcul du MFS CTS

array<int8_t, 256*256> table_tcs_sin;
array<int8_t, 256*256> table_tcs_cos;

void _calcule_tables_tcs()
{
	for(int i = 0; i < 256; ++i)
	{
		for(int j = 0; j < 256; ++j)
		{
			table_tcs_sin.at(256*i+j) = floor(i * sin(float(j) * RAD_PAR_OCT));
			table_tcs_cos.at(256*i+j) = floor(i * cos(float(j) * RAD_PAR_OCT));
		}
	}
}

// Tables pour l'orientation et la norme
array<uint8_t, 97*97> table_orientations;

void _calcule_orientations(uint8_t nb_orientations)
{
	uint8_t orientation;
	for(int dx = -48; dx <= 48; ++dx)
	{
		for(int dy = -48; dy <= 48; ++dy)
		{
			if(dx == 0 && dy == 0)
				orientation = 0;
			else
				orientation = floor((atan2(dy,dx)/(2*PI) + 0.5) * nb_orientations);
			table_orientations.at((dx+48)*49 + dy+48) = orientation;
		}
	}
}

array<uint8_t, 97*97> table_normes;
void _calcule_normes()
{
	uint8_t val;
	for(int dx = -48; dx <= 48; ++dx)
	{
		for(int dy = -48; dy <= 48; ++dy)
		{
			val = (dx*dx + dy*dy) > 0 ? 1 : 0;
			table_normes.at((dx+48)*97 + dy+48) = val;
		}
	}
}









template <int nb_seuils> 
array<Mat, nb_seuils> separe_en_seuils(Mat image)
{
	// Séparation des pixels selon différent seuils
	int pas = 255/nb_seuils;
	int seuils[nb_seuils];

	array<Mat, nb_seuils> ensembles_X;

	for(int i = 0; i < nb_seuils; ++i)
	{
		ensembles_X.at(i) = Mat();
		threshold(image, ensembles_X.at(i), (i+1)*pas, 1, THRESH_BINARY);
	}

	return ensembles_X;
}

array<Mat, 3> calcul_TCS(array<Mat, 3> tab_mat)
{
	Mat canal1 = Mat::zeros(tab_mat.at(1).size(), CV_8UC1);
	Mat canal2 = Mat::zeros(tab_mat.at(1).size(), CV_8UC1);
	for (int i = 0; i < canal1.rows; ++i)
	{
		for (int j = 0; j < canal1.cols; ++j)
		{
			canal1.at<int8_t>(i,j) = table_tcs_cos.at(256*tab_mat[0].at<uint8_t>(i,j)+tab_mat[1].at<uint8_t>(i,j));
			canal2.at<int8_t>(i,j) = table_tcs_sin.at(256*tab_mat[0].at<uint8_t>(i,j)+tab_mat[1].at<uint8_t>(i,j));
		}
	}
	array<Mat, 3> res = {canal1, canal2, tab_mat.at(2)};
	return res;
}




template <int n> 
array<array<Mat, n>, 2> calcul_contours(array<Mat, n>& ensembles_X)
{
	// Calcul des contours
	array<Mat, n> contours;
	array<Mat, n> orientations;
	Size taille = ensembles_X.at(0).size();
	uint16_t tailleX = taille.width;
	uint16_t tailleY = taille.height;
	for(int i = 0; i < n; ++i)
	{
		// Calcul des dérivées
		Mat gradient_x, gradient_y;
		Sobel(ensembles_X.at(i), gradient_x, CV_16SC1, 1, 0, 5);
		Sobel(ensembles_X.at(i), gradient_y, CV_16SC1, 0, 1, 5);

		Mat orientation = Mat(taille, CV_8UC1);
		Mat contour = Mat(taille, CV_8UC1);

		uint16_t pos;
		for(int x = 0; x < tailleX; ++x)
		{
			for(int y = 0; y < tailleY; ++y)
			{
				pos = (gradient_x.at<int16_t>(y,x)+48)*97 + gradient_y.at<int16_t>(y,x)+48;
				orientation.at<uint8_t>(y,x) = table_orientations.at(pos);
				contour.at<uint8_t>(y,x) = table_normes.at(pos);
			}
		}
		orientations.at(i) = orientation;
		contours.at(i) = contour;
	}
	return {contours, orientations};
}





template<int nb_seuils> array<Mat, 2> 
calcul_S_et_O(array<Mat, nb_seuils> contours, array<Mat, nb_seuils> orientations, int nb_orientations)
{
	Size taille = contours.at(0).size();
	int taille_x = taille.height;
	int taille_y = taille.width;
	// Calcul de S et O
	Mat ensemble_S = Mat::zeros(taille, CV_8UC1);
	Mat ensemble_O = Mat::zeros(taille, CV_8UC1);
	uint16_t *nb_occurrences = (uint16_t*) malloc(sizeof(uint16_t)*nb_orientations);

	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			// On calcule l'orientation majoritaire
			for(int i = 0; i < nb_orientations; ++i) nb_occurrences[i] = 0;
			// On compte, pour chaque orientation, le nombre de contours pour lequel il y a cette orientation
			for(int i = 0; i < nb_seuils; ++i)
			{
				if(contours.at(i).template at<uint8_t>(x,y) == 1)
				{
					ensemble_S.at<uint8_t>(x,y)++;
					nb_occurrences[orientations.at(i).template at<uint8_t>(x,y)]++;
				}
			}
			int orientation_majoritaire = 0;
			int nb_orientation_max = 0;
			for(int i = 0; i < nb_orientations; ++i)
			{
				if(nb_occurrences[i] > nb_orientation_max)
				{
					orientation_majoritaire = i;
					nb_orientation_max = nb_occurrences[i];
				}
			}
			// On l'ajoute dans l'ensemble O
			ensemble_O.at<uint8_t>(x,y) = orientation_majoritaire;
		}
	}
	free(nb_occurrences);
	array<Mat, 2> canaux = {ensemble_S, ensemble_O};
	return canaux;
}


array<Mat, 2> synthese_S_O_CTS(array<Mat, 3> ensembles_S, array<Mat, 3> ensembles_O)
{
	// Calcule S et O à partir des différentes valeurs de S et O calculées dans chacun des 3 canaux passés en paramètres.
	Size taille = ensembles_S.at(0).size();
	Mat ensemble_S = Mat::zeros(taille, CV_8UC1);
	Mat ensemble_O = Mat::zeros(taille, CV_8UC1);
	int taille_x = taille.height;
	int taille_y = taille.width;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			int id_max = 0;
			uint8_t max = 0;
			for(int i = 0; i < 3; ++i)
			{
				if(ensembles_S.at(i).at<uint8_t>(x,y) >= max)
				{
					id_max = i;
					max = ensembles_S.at(1).at<uint8_t>(x,y);
				}
			}
			ensemble_S.at<uint8_t>(x,y) = max;
			ensemble_O.at<uint8_t>(x,y) = ensembles_O.at(id_max).at<uint8_t>(x,y);
		}
	}
	array<Mat, 2> res = {ensemble_O, ensemble_S};
	return res;
}

template <int nb_seuils> 
array<Mat, 2> calcule_OS_NB(Mat source, int nb_orientations)
{
	array<Mat, nb_seuils> ensembles_X = separe_en_seuils<nb_seuils>(source);
	array<array<Mat, nb_seuils>, 2> contours = calcul_contours<nb_seuils>(ensembles_X);
	return calcul_S_et_O<nb_seuils>(contours.at(0), contours.at(1), nb_orientations);
}

template <int nb_seuils> 
array<Mat, 2> calcule_OS_CTS(Mat source, int nb_orientations)
{
	array<Mat, 3> canaux = separe_hsv(source);
	array<Mat, 3> ensembles_O;
	array<Mat, 3> ensembles_S;
	array<Mat, 2> ensemble_SO;
	for(int i = 0; i < 3; ++i)
	{
		ensemble_SO = calcule_OS_NB<nb_seuils>(canaux.at(i), nb_orientations);
		ensembles_S.at(i) = ensemble_SO.at(0);
		ensembles_O.at(i) = ensemble_SO.at(1);
	}
	return synthese_S_O_CTS(ensembles_S, ensembles_O);
}
