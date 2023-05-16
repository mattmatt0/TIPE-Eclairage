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
	CommandLineParser parser(argc, argv, "{@image | ../../../../images/lenna.jpg | Image sur laquelle on va opérer nos transformations}");
	Mat image_chargee = imread(parser.get<String>("@image"), IMREAD_GRAYSCALE);
	// On floute un peu l'image pour réduire le bruit
	//GaussianBlur(image_source, image_source, Size(5, 5), 0, 0, BORDER_DEFAULT);
	Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
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

Mat cree_image_orientations(Mat amplitude, Mat orientation, int nb_seuils, int nb_orientations)
{
	Mat orientation_f, amplitude_f;
	orientation.convertTo(orientation_f, CV_32FC1);
	amplitude.convertTo(amplitude_f, CV_32FC1);
	orientation_f *= 180.0/nb_orientations;
	amplitude_f *= 256.0/nb_seuils;
	orientation_f.convertTo(orientation, CV_8UC1);
	amplitude_f.convertTo(amplitude, CV_8UC1);
	Mat res;
	merge(array<Mat, 3>({orientation, Mat(orientation.size(), CV_8UC1, 255), amplitude}), res);
	cvtColor(res, res, COLOR_HSV2BGR);
	return res;
}

Mat cree_image_orientations(Mat orientation, int nb_orientations)
{
	// Pour afficher des orientations uniquement, avec 0 = absence d'orientation.
	// Les orientations sont comprises entre 0 et 180, et doivent donc être stockées sur 8 bits
	return cree_image_orientations(Mat(orientation.size(), CV_8UC1, 1), orientation, 1, nb_orientations);
}

