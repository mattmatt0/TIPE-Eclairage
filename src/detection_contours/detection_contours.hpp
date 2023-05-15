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


template <int nb_seuils> array<Mat, nb_seuils> separe_en_seuils(Mat image)
{
	// Séparation des pixels selon différent seuils
	int pas = 255/nb_seuils;
	int seuils[nb_seuils];

	array<Mat, nb_seuils> ensembles_X;

	for(int i = 0; i < nb_seuils; ++i)
	{
		ensembles_X.at(i) = Mat();
		threshold(image, ensembles_X.at(i), (i+1)*pas, 255, THRESH_BINARY);
	}

	return ensembles_X;
}

array<Mat, 3> calcul_TCS (array<Mat, 3> tab_mat)
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

template <int n> array<Mat, n> calcul_contours(array<Mat, n>& ensembles_X)
{
	// Calcul des contours
	array<Mat, n> contours;
	for(int i = 0; i < n; ++i)
	{
		// Calcul des dérivées
		Mat gradient_x, gradient_y;
		Sobel(ensembles_X.at(i), gradient_x, CV_32FC1, 1, 0, 3);
		Sobel(ensembles_X.at(i), gradient_y, CV_32FC1, 0, 1, 3);

		// Conversion en flottants parce qu'il le faut
		normalize(gradient_x, gradient_x);
		normalize(gradient_y, gradient_y);
		gradient_x *= 256.0;
		gradient_y *= 256.0;

		// Conversion (x,y) -> (norme, direction)
		Mat direction;
		Mat norme;
		phase(gradient_x, gradient_y, direction, true); // "true" pour avoir des degrés	
		magnitude(gradient_x, gradient_y, norme);

		// Décision des pixels qui font partie d'un contour ou non
		Mat contour;
		threshold(norme, contour, 0.5, 1.0, THRESH_BINARY);

		// Ajout de l'orientation dans une matrice à trois canaux 
		// (le 1er représentant l'orientation, le 2e valant 1.0 tout le temps 
		//     et le 3e l'appartenance à un contour)
		array<Mat, 3> canaux = {direction, Mat(direction.size(), CV_32F, 1.0), contour};
		merge(canaux,contours[i]);
}
	return contours;
}

template<int nb_seuils> array<Mat, 2> calcul_S_et_O(array<Mat, nb_seuils> contours)
{
	// Calcul de S et O
	Mat ensemble_S = Mat::zeros(contours.at(0).size(), CV_32F);
	Mat ensemble_O = Mat::zeros(contours.at(0).size(), CV_32F);
	std::array<Mat, nb_seuils> orientations;
	std::array<Mat, nb_seuils> appartient_contours;

	for(int i = 0; i < nb_seuils; ++i)
	{
		array<Mat, 3> hsv; // hsv[2] -> appartenance d'un pixel à la bordure, hsv[0] -> orientation de la bordure en ce pixel
		split(contours.at(i),hsv);
		ensemble_S += 1.0/nb_seuils * hsv.at(2);
		orientations.at(i) = hsv.at(0);
		appartient_contours.at(i) = hsv.at(2);
	}
	int taille_x = contours.at(0).size().height;
	int taille_y = contours.at(0).size().width;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			// On calcule l'orientation majoritaire
			array<int, 360> nb_occurences;
			nb_occurences.fill(0);
			// On compte, pour chaque orientation, le nombre de contours pour lequel il y a cette orientation
			for(int i = 0; i < nb_seuils; ++i)
			{
				if(appartient_contours.at(i).template at<float>(x,y) == 1.0)
					nb_occurences[floor(orientations.at(i).template at<float>(x,y))]++;
			}
			int orientation_majoritaire = 0;
			int nb_orientation_max = 0;
			for(int i = 0; i < 360; ++i)
			{
				if(nb_occurences.at(i) > nb_orientation_max)
				{
					orientation_majoritaire = i;
					nb_orientation_max = nb_occurences.at(i);
				}
			}
			// On l'ajoute dans l'ensemble O
			ensemble_O.at<float>(x,y) = (float) orientation_majoritaire;
		}
	}
	array<Mat, 2> canaux = {ensemble_O, ensemble_S};
	return canaux;
}


array<Mat, 3> synthese_S_O_CTS(array<array<Mat, 2>, 3> canaux)
{
	// Calcule S et O à partir des différentes valeurs de S et O calculées dans chacun des 3 canaux passés en paramètres.
	Mat ensemble_S = Mat::zeros(canaux.at(0).at(0).size(), CV_32F);
	Mat ensemble_O = Mat::zeros(canaux.at(0).at(0).size(), CV_32F);
	int taille_x = canaux.at(0).at(0).size().height;
	int taille_y = canaux.at(0).at(0).size().width;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			int id_max = 0;
			float max = 0;
			for(int i = 0; i < 3; ++i)
			{
				if(canaux.at(i).at(1).at<float>(x,y) >= max)
				{
					id_max = i;
					max = canaux.at(i).at(1).at<float>(x,y);
				}
			}
			ensemble_S.at<float>(x,y) = max;
			ensemble_O.at<float>(x,y) = canaux[id_max].at(0).at<float>(x,y);
		}
	}
	array<Mat, 3> res = {ensemble_O, Mat(ensemble_O.size(), CV_32F, 1.0), ensemble_S};
	return res;

}
