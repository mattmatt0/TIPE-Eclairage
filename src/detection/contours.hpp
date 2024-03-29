#pragma once
#include "outils/outils.hpp"

string const params = params_generiques + params_image + params_analyse + "{mode | rgb | Espace colorimétrique à utiliser: soit ect (Espace de Couleur Texton), soit rgb}";

array<Mat, 3> canaux_hsv(Mat image_hsv)
{
	array<Mat, 3> canaux;
	split(image_hsv, canaux);
	return canaux;
}

// Tables de valeurs pour accélérer le calcul de l'espace de couleur texton (ECT)

array<uint8_t, 256*256> table_ect_sin;
array<uint8_t, 256*256> table_ect_cos;

void _calcul_tables_ect()
{
	for(int i = 0; i < 256; ++i)
	{
		for(int j = 0; j < 256; ++j)
		{
			table_ect_sin.at(256*i+j) = floor(i * sin(float(j) * RAD_PAR_OCT));
			table_ect_cos.at(256*i+j) = floor(i * cos(float(j) * RAD_PAR_OCT));
		}
	}
}

// Tables pour l'orientation et la norme
array<uint8_t, 97*97> table_orientations;

void _calcul_table_orientations(uint8_t nb_orientations)
{
	uint8_t orientation;
	for(int dx = -48; dx <= 48; ++dx)
	{
		for(int dy = -48; dy <= 48; ++dy)
		{
			if(dx == 0 && dy == 0)
				orientation = 0;
			else
				orientation = floor((atan2(dy,dx)/(2*PI) + 0.5) * float(nb_orientations));
			table_orientations.at((dy+48)*97 + dx+48) = orientation;
		}
	}
}

array<uint8_t, 97*97> table_normes;
void _calcul_table_normes()
{
	uint8_t val;
	for(int dx = -48; dx <= 48; ++dx)
	{
		for(int dy = -48; dy <= 48; ++dy)
		{
			val = (dx*dx + dy*dy) > 2000 ? 1 : 0;
			table_normes.at((dy+48)*97 + dx+48) = val;
		}
	}
}

array<uint8_t, 256> table_seuils; // associe à toutes les intensités possibles le numéro associé
void _calcul_table_seuils(uint8_t nb_seuils)
{
	int pas = 255/nb_seuils;
	int i = pas;
	for(int p = 0; p < nb_seuils; ++p)
	{
		while(i <= (p+2)*pas && i < 256)
		{
			table_seuils.at(i) = p;
			++i;
		}
	}
	for(; i < 256; ++i) table_seuils.at(i) = nb_seuils-1;
}

Mat *_seuils_rapide(Mat m, uint8_t nb_seuils)
{
	Size taille = m.size();
	int taille_x = taille.width;
	int taille_y = taille.height;
	Mat *res = new Mat[nb_seuils];
	for(int i = 0; i < nb_seuils; ++i) res[i] = Mat::zeros(taille, CV_8UC1);
	// Précalcul: seuils
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			for(int i = 0; i <= table_seuils.at(m.at<uint8_t>(y,x)); ++i)
			{
				res[i].at<uint8_t>(y,x) = 1;
			}
		}
	}
	return res;
}

void _calcul_table_seuils_moderne(uint8_t nb_seuils)
{
	for(int i = 0; i < 256; ++i)
		table_seuils.at(i) = floor(i/256.0 * nb_seuils);
}


Mat *seuils(Mat image, int nb_seuils)
{
	// Séparation des pixels selon différent seuils
	int pas = 255/nb_seuils;
	Mat *ensembles_X = new Mat[nb_seuils];

	for(int i = 0; i < nb_seuils; ++i)
	{
		threshold(image, ensembles_X[i], (i+1)*pas, 1, THRESH_BINARY);
	}

	return ensembles_X;
}

array<Mat, 3> calcul_ECT(array<Mat, 3> tab_mat)
{
	Size taille = tab_mat.at(1).size();
	Mat canal1 = Mat(taille, CV_8UC1);
	Mat canal2 = Mat(taille, CV_8UC1);
	int taille_X = taille.width;
	int taille_Y = taille.height;
	for (int x = 0; x < taille_X; ++x)
	{
		for (int y = 0; y < taille_Y; ++y)
		{
			canal1.at<uint8_t>(y,x) = table_ect_cos.at(256*tab_mat[0].at<uint8_t>(y,x)+tab_mat[1].at<uint8_t>(y,x));
			canal2.at<uint8_t>(y,x) = table_ect_sin.at(256*tab_mat[0].at<uint8_t>(y,x)+tab_mat[1].at<uint8_t>(y,x));
		}
	}
	array<Mat, 3> res = {canal1, canal2, tab_mat.at(2)};
	return res;
}




array<Mat*,2> calcul_contours(Mat* ensembles_X, int nb_seuils)
{
	// Calcul des contours
	Mat* contours = new Mat[nb_seuils];
	Mat* orientations = new Mat[nb_seuils];
	Size taille = ensembles_X[0].size();
	uint16_t tailleX = taille.width;
	uint16_t tailleY = taille.height;
	for(int i = 0; i < nb_seuils; ++i)
	{
		// Calcul des dérivées
		Mat gradient_x, gradient_y;
		Sobel(ensembles_X[i], gradient_x, CV_16SC1, 1, 0, 5);
		Sobel(ensembles_X[i], gradient_y, CV_16SC1, 0, 1, 5);

		Mat orientation = Mat(taille, CV_8UC1);
		Mat contour = Mat(taille, CV_8UC1);

		uint16_t pos;
		for(int x = 0; x < tailleX; ++x)
		{
			for(int y = 0; y < tailleY; ++y)
			{
				pos = (gradient_y.at<int16_t>(y,x)+48)*97 + gradient_x.at<int16_t>(y,x)+48;
				orientation.at<uint8_t>(y,x) = table_orientations.at(pos);
				contour.at<uint8_t>(y,x) = table_normes.at(pos);
			}
		}
		orientations[i] = orientation;
		contours[i] = contour;
	}
	return {contours, orientations};
}

int8_t sobel_kernel_x[25] = {-1, -2, 0, 2, 1, -4, -8, 0, 8, 4, -6, -12, 0, 12, 6, -4, -8, 0, 8, 4, -1, -2, 0, 2, 1};
int8_t sobel_kernel_y[25] = {-1, -4, -6, -4, -1, -2, -8, -12, -8, -2, 0, 0, 0, 0, 0, 2, 8, 12, 8, 2, 1, 4, 6, 4, 1};

array<Mat, 2> calcul_SO_rapide(Mat image_source, int nb_seuils, int nb_orientations)
{
	Size taille = image_source.size();
	int taille_x = taille.width;
	int taille_y = taille.height;
	// Précalcul: seuils
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			image_source.at<uint8_t>(y,x) = table_seuils.at(image_source.at<uint8_t>(y,x));
		}
	}

	Mat m;
	copyMakeBorder(image_source, m, 2, 2, 2, 2, BORDER_DEFAULT);
	int8_t *somme_seuils_x = (int8_t*) calloc(nb_seuils, sizeof(int8_t));
	int8_t *somme_seuils_y = (int8_t*) calloc(nb_seuils, sizeof(int8_t));
	uint8_t *nb_occurrences_orientations = (uint8_t*) calloc(nb_orientations, sizeof(uint8_t));
	uint8_t valeur_courante;
	int8_t gradient_x, gradient_y;
	uint8_t nb_lignes_courant;
	uint8_t orientation_majoritaire;
	uint8_t nb_occurrences_orientation_maj;
	uint16_t pos;
	array<Mat, 2> res;
	res.at(0) = Mat(taille, CV_8UC1);
	res.at(1) = Mat(taille, CV_8UC1);
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			// Récupération des valeurs présentes dans la zone étudiée
			for(uint8_t dx = 0; dx < 5; ++dx)
			{
				for(uint8_t dy = 0; dy < 5; ++dy)
				{
					valeur_courante = m.at<uint8_t>(y+dy,x+dx);
					assert(valeur_courante < nb_seuils && valeur_courante >= 0);
					somme_seuils_x[valeur_courante] += sobel_kernel_x[dy*5+dx];
					somme_seuils_y[valeur_courante] += sobel_kernel_y[dy*5+dx];
				}
			}

			// Calcul des valeurs des dérivées pour chaque seuil, conclusion du nombre de lignes de contour et l'orientation
			
			gradient_x = 0;
			gradient_y = 0;
			nb_lignes_courant = 0;
			for(int i = nb_seuils-1; i >= 0; --i)
			{
				gradient_x += somme_seuils_x[i];
				gradient_y += somme_seuils_y[i];
				somme_seuils_x[i] = 0;
				somme_seuils_y[i] = 0;
				pos = (gradient_y+48)*97 + gradient_x + 48;
				nb_lignes_courant += table_normes[pos];
				assert(97*97 > pos && pos >= 0);
				nb_occurrences_orientations[table_orientations.at(pos)] += table_normes.at(pos);
			}

			// Détermination de l'orientation majoritaire
			orientation_majoritaire = 0;
			nb_occurrences_orientation_maj = 0;
			for(int i = 0; i < nb_orientations; ++i)
			{
				if(nb_occurrences_orientations[i] > nb_occurrences_orientation_maj)
				{
					nb_occurrences_orientation_maj = nb_occurrences_orientations[i];
					orientation_majoritaire = i;
				}
				nb_occurrences_orientations[i] = 0;
			}

			// Conclusion sur la valeur
			//if(nb_lignes_courant != 0 || orientation_majoritaire) cout << nb_lignes_courant << " " << orientation_majoritaire << endl;
			res.at(0).at<uint8_t>(y,x) = nb_lignes_courant;
			res.at(1).at<uint8_t>(y,x) = orientation_majoritaire;
		}
	}
	free(somme_seuils_x);
	free(somme_seuils_y);
	free(nb_occurrences_orientations);
	return res;
}


array<Mat, 2> calcul_S_et_O(Mat* contours, Mat* orientations, int nb_seuils, int nb_orientations)
{
	Size taille = contours[0].size();
	int taille_x = taille.width;
	int taille_y = taille.height;
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
				if(contours[i].at<uint8_t>(y,x) == 1)
				{
					ensemble_S.at<uint8_t>(y,x)++;
					nb_occurrences[orientations[i].at<uint8_t>(y,x)]++;
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
			ensemble_O.at<uint8_t>(y,x) = orientation_majoritaire;
		}
	}
	free(nb_occurrences);
	array<Mat, 2> canaux = {ensemble_S, ensemble_O};
	return canaux;
}


array<Mat, 2> synthese_S_O_ECT(array<Mat, 3> ensembles_S, array<Mat, 3> ensembles_O)
{
	// Calcule S et O à partir des différentes valeurs de S et O calculées dans chacun des 3 canaux passés en paramètres.
	Size taille = ensembles_S.at(0).size();
	Mat ensemble_S = Mat(taille, CV_8UC1);
	Mat ensemble_O = Mat(taille, CV_8UC1);
	int taille_x = taille.width;
	int taille_y = taille.height;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			int id_max = 0;
			uint8_t max = 0;
			for(int i = 0; i < 3; ++i)
			{
				if(ensembles_S.at(i).at<uint8_t>(y,x) > max)
				{
					id_max = i;
					max = ensembles_S.at(i).at<uint8_t>(y,x);
				}
			}
			ensemble_S.at<uint8_t>(y,x) = max;
			ensemble_O.at<uint8_t>(y,x) = ensembles_O.at(id_max).at<uint8_t>(y,x);
		}
	}
	array<Mat, 2> res = {ensemble_S, ensemble_O};
	return res;
}

array<Mat, 2> calcul_SO_NB(Mat source, int nb_seuils, int nb_orientations)
{
	Mat* ensembles_X = seuils(source,nb_seuils);
	array<Mat*, 2> contours = calcul_contours(ensembles_X, nb_seuils);
	delete [] ensembles_X;
	array<Mat, 2> res = calcul_S_et_O(contours.at(0), contours.at(1), nb_seuils, nb_orientations);
	delete [] contours.at(0);
	delete [] contours.at(1);
	return res;
}

array<Mat, 2> calcul_SO_ECT(Mat source, int nb_seuils, int nb_orientations)
{
	array<Mat, 3> canaux = calcul_ECT(canaux_hsv(source));
	array<Mat, 3> ensembles_O;
	array<Mat, 3> ensembles_S;
	array<Mat, 2> ensemble_SO;
	for(int i = 0; i < 3; ++i)
	{
		ensemble_SO = calcul_SO_NB(canaux.at(i), nb_seuils, nb_orientations);
		ensembles_S.at(i) = ensemble_SO.at(0);
		ensembles_O.at(i) = ensemble_SO.at(1);

	}
	return synthese_S_O_ECT(ensembles_S, ensembles_O);
}
