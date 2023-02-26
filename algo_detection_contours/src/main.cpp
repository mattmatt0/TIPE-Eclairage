#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../img.png | Image sur laquelle on va opérer nos transformations}");
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

		cv::Mat contour;
		cv::threshold(norme, contour, 0.5, 1.0, cv::THRESH_BINARY);

		std::vector<cv::Mat> canaux;
		canaux.push_back(direction);
		canaux.push_back(cv::Mat(direction.size(), CV_32F, 1.0));
		canaux.push_back(contour);
		cv::Mat gradient;
		cv::merge(canaux,contours[i]);
	}

	// Calcul de S
	cv::Mat ensemble_S = cv::Mat::zeros(contours[0].size(), CV_32F);
	for(int i = 0; i < NB_SEUILS; ++i)
	{
		cv::Mat hsv[3];
		split(contours[i],hsv);
		ensemble_S += 1.0/NB_SEUILS * hsv[2];
	}

	// TODO: Calcul de O


	// On affiche les deux ensembles
	cv::imshow("ensemble S", ensemble_S);

	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
