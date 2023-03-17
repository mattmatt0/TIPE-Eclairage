#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../images/toy_example.jpg | Image sur laquelle on va opérer nos transformations}");
	cv::Mat image_chargee = imread(parser.get<cv::String>( "@image" ), cv::IMREAD_GRAYSCALE);

	// On floute un peu l'image pour réduire le bruit
	//GaussianBlur(image_source, image_source, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	cv::Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
	

	// Première étape: séparation des pixels selon différent seuils:
	int const NB_SEUILS = 10;
	int seuils[NB_SEUILS] = {5,10,20,30,50,70,100,150,200,250};
	cv::Mat ensembles_X[NB_SEUILS];
	cv::Mat contours[NB_SEUILS];
	

	for(int i = 0; i < NB_SEUILS; ++i)
		cv::threshold(image_source, ensembles_X[i], seuils[i], 255, cv::THRESH_BINARY);

	
	// Calcul des contours
	for(int i = 0; i < NB_SEUILS; ++i)
	{
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

	// Calcul de S et O
	cv::Mat ensemble_S = cv::Mat::zeros(contours[0].size(), CV_32F);
	cv::Mat ensemble_O = cv::Mat::zeros(contours[0].size(), CV_32F);
	cv::Mat orientations[NB_SEUILS];
	cv::Mat appartient_contours[NB_SEUILS];

	for(int i = 0; i < NB_SEUILS; ++i)
	{
		cv::Mat hsv[3]; // hsv[2] -> appartenance d'un pixel à la bordure, hsv[0] -> orientation de la bordure en ce pixel
		split(contours[i],hsv);
		ensemble_S += 1.0/NB_SEUILS * hsv[2];
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
			for(int i = 0; i < NB_SEUILS; ++i)
			{
				if(appartient_contours[i].at<float>(x,y) == 1.0)
				//{
					nb_occurences[floor(orientations[i].at<float>(x,y))]++;
		//		}
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
	std::vector<cv::Mat> canaux_2;
	canaux_2.push_back(ensemble_O);
	canaux_2.push_back(cv::Mat(ensemble_O.size(), CV_32F, 1.0));
	canaux_2.push_back(ensemble_S);
	cv::Mat image_synthese;
	cv::merge(canaux_2, image_synthese);
	cv::cvtColor(image_synthese, image_synthese, cv::COLOR_HSV2BGR);


	// On affiche les deux ensembles
	cv::imshow("ensemble S", ensemble_S);
	cv::imshow("ensemble O", ensemble_O);
	cv::imshow("Synthese", image_synthese);

	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
