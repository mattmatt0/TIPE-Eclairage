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

struct TroisCanaux
{
	Mat canal1;
	Mat canal2;
	Mat canal3;	
};

Mat charge_image(int argc, char** argv)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	CommandLineParser parser(argc, argv, "{@image | ../../../../images/toy_example.jpg | Image sur laquelle on va opérer nos transformations}");
	Mat image_chargee = imread(parser.get<String>( "@image" ), IMREAD_GRAYSCALE);
	
	// On floute un peu l'image pour réduire le bruit
	//GaussianBlur(image_source, image_source, Size(5, 5), 0, 0, BORDER_DEFAULT);
	Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
	return image_source;
}

Mat charge_image_hsv(int argc, char** argv)
{
	// On charge l'image donnée en paramètre du programme dans l'espace de couleur BGR
	CommandLineParser parser(argc, argv, "{@image | ../../../../images/toy_example.jpg | Image sur laquelle on va opérer nos transformations}");
	Mat image_chargee = imread(parser.get<String>( "@image" ), IMREAD_COLOR);
	// On convertit l'image dans l'espace de couleur HSV
	Mat image_source;
	cvtColor(image_chargee, image_source, COLOR_BGR2HSV);
	return image_source; 
}

// Coder fonction pour le calcul de Texton Color Space (TCS)
// Définir un nouveau type struct pour renvoyer les 3 canaux

struct TroisCanaux hsv2SplitedChannels (Mat image_hsv)
{
	Mat splited[3];
	split(image_hsv, splited);
	struct TroisCanaux image_hsv_splited = {splited[0], splited[1], splited[2]};
	return image_hsv_splited;
}

struct TroisCanaux calcul_TCS (struct TroisCanaux tab_mat)
{
	Mat canal1 = Mat::zeros(tab_mat.canal2.size(), CV_8UC1);
	Mat canal2 = Mat::zeros(tab_mat.canal2.size(), CV_8UC1);
	for (int i = 0; i < canal1.rows; ++i)
	{
		for (int j = 0; j < canal1.cols; ++j)
		{
			canal1.at<uint8_t>(i,j) = floor(tab_mat.canal2.at<uint8_t>(i,j) * sin(float(tab_mat.canal1.at<uint8_t>(i,j))/180));
			canal2.at<uint8_t>(i,j) = floor(tab_mat.canal2.at<uint8_t>(i,j) * cos(float(tab_mat.canal1.at<uint8_t>(i,j))/180));
		}
	}
	struct TroisCanaux TCS = {canal1, canal2, tab_mat.canal3};
	return TCS;
}

vector<Mat> separe_en_seuils(Mat image, int nb_seuils)
{
	// Séparation des pixels selon différent seuils
	int pas = 250/nb_seuils;
	int seuils[nb_seuils];

	vector<Mat> ensembles_X;

	for(int i = 0; i < nb_seuils; ++i)
	{
		ensembles_X.push_back(Mat());
		threshold(image, ensembles_X.at(i), (i+1)*pas, 255, THRESH_BINARY);
	}

	return ensembles_X;
}

vector<Mat> calcul_contours(Mat image, vector<Mat> ensembles_X, int nb_seuils)
{
	// Calcul des contours
	vector<Mat> contours;
	for(int i = 0; i < nb_seuils; ++i)
	{
		contours.push_back(Mat());

		// Calcul des dérivées
		Mat gradient_x, gradient_y;
		Sobel(ensembles_X[i], gradient_x, CV_32F, 1, 0, 3);
		Sobel(ensembles_X[i], gradient_y, CV_32F, 0, 1, 3);

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
		vector<Mat> canaux;
		canaux.push_back(direction);
		canaux.push_back(Mat(direction.size(), CV_32F, 1.0));
		canaux.push_back(contour);
		Mat gradient;
		merge(canaux,contours[i]);
}
	return contours;
}

vector<Mat> calcul_S_et_O(vector<Mat> contours, int nb_seuils)
{
	// Calcul de S et O
	Mat ensemble_S = Mat::zeros(contours[0].size(), CV_32F);
	Mat ensemble_O = Mat::zeros(contours[0].size(), CV_32F);
	Mat orientations[nb_seuils];
	Mat appartient_contours[nb_seuils];

	for(int i = 0; i < nb_seuils; ++i)
	{
		Mat hsv[3]; // hsv[2] -> appartenance d'un pixel à la bordure, hsv[0] -> orientation de la bordure en ce pixel
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
			array<int, 360> nb_occurences;
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
	vector<Mat> canaux;
	canaux.push_back(ensemble_O);
	canaux.push_back(Mat(ensemble_O.size(), CV_32F, 1.0));
	canaux.push_back(ensemble_S);
	return canaux;
}
