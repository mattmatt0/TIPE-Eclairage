#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../img.png | Image sur laquelle on va opérer nos transformations}");

	// On floute un peu l'image pour réduire le bruit
	cv::Mat image_chargee = imread(parser.get<cv::String>( "@image" ), cv::IMREAD_GRAYSCALE);
	//GaussianBlur(image_source, image_source, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
	cv::Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);

	// Première étape: séparation des pixels selon différent seuils:
	cv::Mat ensemble_niveau = cv::Mat::zeros(image_source.size(), CV_8UC3);
	cv::threshold(image_source, ensemble_niveau, 100, 255, cv::THRESH_BINARY);

	// Calcul des dérivées
	cv::Mat gradient_x, gradient_y;
	Sobel(ensemble_niveau, gradient_x, CV_32F, 1, 0, 3);
	Sobel(ensemble_niveau, gradient_y, CV_32F, 0, 1, 3);

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
	cv::threshold(norme, contour, 0.5, 255, cv::THRESH_BINARY);

	// Création de l'image représentant le gradient
	std::vector<cv::Mat> canaux;
	canaux.push_back(direction);
	canaux.push_back(cv::Mat(direction.size(), CV_32F, 1.0));
	canaux.push_back(norme);
	cv::Mat gradient;
	cv::merge(canaux,gradient);
	cv::cvtColor(gradient, gradient, cv::COLOR_HSV2BGR);

	// On affiche l'image
	cv::imshow("image", ensemble_niveau);
	cv::imshow("Gradient X", gradient_x);
	cv::imshow("Gradient Y", gradient_y);
	cv::imshow("Gradient", gradient);
	cv::imshow("Contour",contour);
	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
