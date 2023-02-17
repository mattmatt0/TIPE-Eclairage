#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	cv::RNG rng;
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../img.png | Image sur laquelle on va opérer nos transformations}");
	cv::Mat image_source = imread(parser.get<cv::String>( "@image" ), cv::IMREAD_GRAYSCALE);

	// Première étape: séparation des pixels selon différent seuils:
	cv::threshold(image_source, image_source, 100, 255, cv::THRESH_BINARY);

	// Deuxième étape: détection des contours:
	cv::Mat drawing = cv::Mat::zeros(image_source.size(), CV_8UC3);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(image_source, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	for(int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255,0,0);
		cv::drawContours(drawing, contours, i, color, 1, cv::LINE_8, hierarchy, 0);
	}

	// On affiche l'image
	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
	cv::imshow("image", image_source);
	cv::imshow( "Contours", drawing);
	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
