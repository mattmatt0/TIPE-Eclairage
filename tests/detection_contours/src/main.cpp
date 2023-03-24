#include "detection_contours/detection_contours.hpp"
#include <iostream>

cv::Mat charge_image(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../../images/toy_example.jpg | Image sur laquelle on va opérer nos transformations}");
	cv::Mat image_chargee = imread(parser.get<cv::String>( "@image" ), cv::IMREAD_GRAYSCALE);
	
	// On floute un peu l'image pour réduire le bruit
	//GaussianBlur(image_source, image_source, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	cv::Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
	return image_source;
}

std::vector<cv::Mat> separe_en_seuils(cv::Mat image, int nb_seuils)
{
	// Séparation des pixels selon différent seuils
	int pas = 250/nb_seuils;
	int seuils[nb_seuils];

	std::vector<cv::Mat> ensembles_X;

	for(int i = 0; i < nb_seuils; ++i)
	{
		ensembles_X.push_back(cv::Mat());
		cv::threshold(image, ensembles_X.at(i), (i+1)*pas, 255, cv::THRESH_BINARY);
	}

	return ensembles_X;
}

std::vector<cv::Mat> calcul_contours(cv::Mat image, std::vector<cv::Mat> ensembles_X, int nb_seuils)
{
	// Calcul des contours
	std::vector<cv::Mat> contours;
	for(int i = 0; i < nb_seuils; ++i)
	{
		contours.push_back(cv::Mat());

		// Calcul des dérivées
		cv::Mat gradient_x, gradient_y;
		Sobel(ensembles_X[i], gradient_x, CV_32F, 1, 0, 3);
		Sobel(ensembles_X[i], gradient_y, CV_32F, 0, 1, 3);

		// Conversion en flottants parce qu'il le faut
		cv::normalize(gradient_x, gradient_x);
		cv::normalize(gradient_y, gradient_y);
		gradient_x *= 256.0;
		gradient_y *= 256.0;

		// Conversion (x,y) -> (norme, direction)
		cv::Mat direction;
		cv::Mat norme;
		cv::phase(gradient_x, gradient_y, direction, true); // "true" pour avoir des degrés	
		cv::magnitude(gradient_x, gradient_y, norme);

		// Décision des pixels qui font partie d'un contour ou non
		cv::Mat contour;
		cv::threshold(norme, contour, 0.5, 1.0, cv::THRESH_BINARY);

		// Ajout de l'orientation dans une matrice à trois canaux 
		// (le 1er représentant l'orientation, le 2e valant 1.0 tout le temps 
		//     et le 3e l'appartenance à un contour)
		std::vector<cv::Mat> canaux;
		canaux.push_back(direction);
		canaux.push_back(cv::Mat(direction.size(), CV_32F, 1.0));
		canaux.push_back(contour);
		cv::Mat gradient;
		cv::merge(canaux,contours[i]);
	}
	return contours;
}

std::vector<cv::Mat> calcul_S_et_O(std::vector<cv::Mat> contours, int nb_seuils)
{
	// Calcul de S et O
	cv::Mat ensemble_S = cv::Mat::zeros(contours[0].size(), CV_32F);
	cv::Mat ensemble_O = cv::Mat::zeros(contours[0].size(), CV_32F);
	cv::Mat orientations[nb_seuils];
	cv::Mat appartient_contours[nb_seuils];

	for(int i = 0; i < nb_seuils; ++i)
	{
		cv::Mat hsv[3]; // hsv[2] -> appartenance d'un pixel à la bordure, hsv[0] -> orientation de la bordure en ce pixel
		split(contours[i],hsv);
		ensemble_S += 1.0/nb_seuils * hsv[2];
		orientations[i] = hsv[0];
		appartient_contours[i] = hsv[2];
	}
	int taille_x = contours[0].size().height;
	int taille_y = contours[0].size().width;
	for(int x = 0; x < taille_x; ++x)
	{
		for(int y = 0; y < taille_y; ++y)
		{
			// On calcule l'orientation majoritaire
			std::array<int, 360> nb_occurences;
			nb_occurences.fill(0);
			// On compte, pour chaque orientation, le nombre de contours pour lequel il y a cette orientation
			for(int i = 0; i < nb_seuils; ++i)
			{
				if(appartient_contours[i].at<float>(x,y) == 1.0)
					nb_occurences[floor(orientations[i].at<float>(x,y))]++;
			}
			int orientation_majoritaire = 0;
			int nb_orientation_max = 0;
			for(int i = 0; i < 360; ++i)
			{
				if(nb_occurences[i] > nb_orientation_max)
				{
					orientation_majoritaire = i;
					nb_orientation_max = nb_occurences[i];
				}
			}
			// On l'ajoute dans l'ensemble O
			ensemble_O.at<float>(x,y) = (float) orientation_majoritaire;
		}
	}
	std::vector<cv::Mat> canaux;
	canaux.push_back(ensemble_O);
	canaux.push_back(cv::Mat(ensemble_O.size(), CV_32F, 1.0));
	canaux.push_back(ensemble_S);
	return canaux;
}


int main(int argc, char** argv)
{	
	cv::Mat image_source = charge_image(argc, argv);

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 10;

	// Séparation des pixels répartits dans les nb_seuils seuils.
	std::vector<cv::Mat> ensembles_X = separe_en_seuils(image_source, NB_SEUILS); 

	// Calcul des contours
	std::vector<cv::Mat> contours = calcul_contours(image_source, ensembles_X, NB_SEUILS);

	// Calcul des canaux hsv
	std::vector<cv::Mat> canaux = calcul_S_et_O(contours, NB_SEUILS);
	cv::Mat ensemble_O = canaux.at(0);
	cv::Mat ensemble_S = canaux.at(2);
	
	// Calcul de l'image de synthèse
	cv::Mat image_synthese;
	cv::merge(canaux, image_synthese);
	cv::cvtColor(image_synthese, image_synthese, cv::COLOR_HSV2BGR);


	// On affiche les deux ensembles
	cv::imshow("ensemble S", ensemble_S);
	cv::imshow("ensemble O", ensemble_O);
	cv::imshow("Synthese", image_synthese);

	int const PERIOD = 2;
	cv::Mat reference_model[PERIOD];
	
	
	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
