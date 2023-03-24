#include "detection_contours/detection_contours.hpp"
#include <iostream>
	
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
