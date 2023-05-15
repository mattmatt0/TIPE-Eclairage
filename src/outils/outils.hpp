#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

string type2str(int type) 
{
	string r;
	uchar profondeur = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);
	switch(profondeur) 
	{
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
	}
	r += "C";
	r += (chans+'0');
	return r;
}

vector<Mat> charge_repertoire_images(string repertoire, string extension, int limite = -1)
{
	// Charge toutes les images dans `repertoire` avec l'extension `extension`
	// `repertoire` ne doit pas comporter de "/" à la fin.
	// `extension` ne doit pas comporter de "." au début
	vector<cv::String> emplacements;
	cout << "Recherche d'images dans " << repertoire + "/*." + extension << endl;
	glob(repertoire + "/*." + extension, emplacements, false);

	vector<Mat> images;
	int nb_images = (limite == -1) ? emplacements.size() : limite; 
	Mat image_courante;

	for(int i = 0; i < nb_images; ++i)
	{
		cout << "Lecture de " + emplacements[i] << endl;
		image_courante = imread(emplacements[i], IMREAD_GRAYSCALE);
		Mat img_filtree;
		bilateralFilter(image_courante, img_filtree, 10, 20, 5);
    	images.push_back(img_filtree);
	}
	
	return images;
}

Mat charge_image(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	CommandLineParser parser(argc, argv, "{@image | ../../../../images/lane.jpg | Image sur laquelle on va opérer nos transformations}");
	Mat image_chargee = imread(parser.get<String>("@image"), IMREAD_GRAYSCALE);
	// On floute un peu l'image pour réduire le bruit
	Mat image_source;
	GaussianBlur(image_chargee, image_source, Size(5, 5), 0, 0, BORDER_DEFAULT);
	//bilateralFilter(image_chargee, image_source, 10, 20, 5);
	return image_source;
}

Mat charge_image_hsv(int argc, char** argv)
{
	// On charge l'image donnée en paramètre du programme dans l'espace de couleur BGR
	CommandLineParser parser(argc, argv, "{@image | ../../../../images/lenna.jpg | Image sur laquelle on va opérer nos transformations}");
	Mat image_chargee = imread(parser.get<String>( "@image" ), IMREAD_COLOR);
	// On convertit l'image dans l'espace de couleur HSV
	Mat image_source;
	cvtColor(image_chargee, image_source, COLOR_BGR2HSV);
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
	return image_source; 
}

Mat cree_image_orientations(Mat orientations)
{
	// Pour afficher des orientations uniquement, avec 0 = absence d'orientation.
	// Les orientations sont comprises entre 0 et 180, et doivent donc être stockées sur 8 bits

	Mat intensites = Mat(orientations.size(), CV_8UC1);
	Mat saturations = Mat(orientations.size(), CV_8UC1);
	for(int x = 0; x < orientations.size().height; ++x)
	{
		for(int y = 0; y < orientations.size().width; ++y)
		{
			if(orientations.at<uint8_t>(x,y) == 0) intensites.at<uint8_t>(x,y) = 0;
			else intensites.at<uint8_t>(x,y) = 255;
			saturations.at<uint8_t>(x,y) = 255;
		}
	}
	//imshow("intensites",intensites);
	vector<Mat> canaux;
	canaux.push_back(orientations);
	canaux.push_back(saturations);
	canaux.push_back(intensites);
	Mat res;
	merge(canaux, res);
	cvtColor(res, res, COLOR_HSV2BGR);
	return res;
}
