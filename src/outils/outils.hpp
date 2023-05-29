#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

#include "outils/constantes.hpp"

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

vector<Mat> chargement_repertoire_images(string repertoire, string extension, int limite = -1, vector<string>* noms_fichiers = NULL)
{
	// Charge toutes les images dans `repertoire` avec l'extension `extension`
	// `repertoire` ne doit pas comporter de "/" à la fin.
	// `extension` ne doit pas comporter de "." au début
	vector<string> emplacements;
	cout << "Chargement des images d'emplacement " << repertoire + "/*." + extension << endl;
	glob(repertoire + "/*." + extension, emplacements, false);
	

	vector<Mat> images;
	int nb_images = (limite == -1) ? emplacements.size() : limite; 
	Mat image_courante;

	if(noms_fichiers)
	{
		int taille_prefixe = repertoire.size()+1;
		for(int i = 0; i < nb_images; ++i)
		{
			noms_fichiers->push_back(emplacements.at(i).substr(taille_prefixe, string::npos));
		}
	}

	for(int i = 0; i < nb_images; ++i)
	{
		image_courante = imread(emplacements[i], IMREAD_GRAYSCALE);
		Mat img_filtree;
		bilateralFilter(image_courante, img_filtree, 10, 20, 5);
    	images.push_back(img_filtree);
	}
	cout << nb_images << " images lues" << endl;
	
	return images;
}

Mat chargement_image(string emplacement)
{
	// On charge l'image donnée en paramètre (par défaut img.png dans le dossier source) en noir et blanc
	Mat image_chargee = imread(emplacement, IMREAD_GRAYSCALE);
	// On floute un peu l'image pour réduire le bruit
	Mat image_source;
	bilateralFilter(image_chargee, image_source, 10, 20, 5);
	return image_source;
}

Mat chargement_image_hsv(string emplacement)
{
	// On charge l'image donnée en paramètre du programme dans l'espace de couleur BGR
	Mat image_1 = imread(emplacement, IMREAD_COLOR);
	// On convertit l'image dans l'espace de couleur HSV
	Mat image_2;
	cvtColor(image_1, image_2, COLOR_BGR2HSV);
	bilateralFilter(image_2, image_1, 10, 20, 5);
	return image_1; 
}

Mat image_orientations(Mat amplitude, Mat orientation, int nb_seuils, int nb_orientations)
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

Mat image_orientations(Mat orientation, int nb_orientations)
{
	// Pour afficher des orientations uniquement, avec 0 = absence d'orientation.
	// Les orientations sont comprises entre 0 et 180, et doivent donc être stockées sur 8 bits
	return image_orientations(Mat(orientation.size(), CV_8UC1, 1), orientation, 1, nb_orientations);
}


string nombre_taille_fixe(unsigned int nb, size_t taille)
{
	if(nb == 0) return String(taille, '0');
	string res = "";
	while(nb != 0)
	{
		res += (char) (nb % 10 + 48);
		--taille;
		nb /= 10;
	}
	res += String(taille, '0');
	reverse(res.begin(), res.end());
	return res;
}

void attend_q() { while(waitKeyEx() != TOUCHE_Q); }

void affiche_image_debug(Mat amplitude, Mat orientation, int nb_seuils, int nb_orientations, string nom = "Debug Image")
{
	imshow(nom, image_orientations(amplitude, orientation, nb_seuils, nb_orientations));
	attend_q();
}

void affiche_image_debug(Mat orientation, int nb_orientations, string nom = "Debug Image")
{
	imshow(nom, image_orientations(orientation, nb_orientations));
	attend_q();
}

void affiche_noyau_sobel(int ordre_x, int ordre_y, int taille)
{
	cout << ordre_x << " " << ordre_y << " " << taille << endl;
	Mat noyau_x, noyau_y;
	getDerivKernels(noyau_x, noyau_y, ordre_x, ordre_y, taille);
	Mat noyau_entier_x, noyau_entier_y;
	noyau_x.convertTo(noyau_entier_x, CV_32SC1);
	noyau_y.convertTo(noyau_entier_y, CV_32SC1);
	cout << noyau_y*noyau_x.t()<< endl;
}


Mat precalcul_integral(Mat image_source)
{
	// Hyp : image_source est en noir et blanc (0 ou 255)
	// Renvoie la matrice avec les valeurs précalculées pour le calcul d'image intégrale
	int n = image_source.rows;
	int m = image_source.cols;
	Mat image_integrale = Mat::zeros(n, m, CV_32F);
	
	for (int i = 0; i < n; ++i)
	{
		image_integrale.at<int>(i,0) = image_source.at<int>(i,0);
	} 

	for (int i = 0; i < n; ++i)
	{
		for (int j = 1; j < m; ++j)
		{
			image_integrale.at<int>(i,j) = image_source.at<int>(i,j) + image_integrale.at<int>(i,j-1);
		}
	}

	for (int j = 0; j < m; ++j)
	{
		for (int i = 1; i < n; ++i)
		{
			image_integrale.at<int>(i,j) = image_integrale.at<int>(i,j) + image_integrale.at<int>(i-1,j);
		}
	}

	return image_integrale;
}


int calcul_integral(Mat image_source, int x1, int y1, int x2, int y2)
{
	// Calcul image intégrale dans le rectangle de coin supérieur gauche (x1,y1) et de coin inférieur droit (x2,y2)
	Mat image_integrale = precalcul_integral(image_source);
	int res = image_integrale.at<int>(x2,y2) - image_integrale.at<int>(x1,y2) - image_integrale.at<int>(x2,y1) + image_integrale.at<int>(x1,y1);
	return res;
}
