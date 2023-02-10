#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	cv::CommandLineParser parser(argc, argv, "{@image | ../../../img.png | Image sur laquelle on va opérer nos transformations}");
	cv::Mat image_source = imread(parser.get<cv::String>( "@image" ), cv::IMREAD_GRAYSCALE);

	// On affiche l'image
	cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
	cv::imshow("image", image_source);
	
	// On attend que la touche `q` ait été pressée
	while(cv::waitKeyEx() != 113);
	return 0;
}
