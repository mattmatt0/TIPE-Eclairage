#include "detection_contours/detection_contours.hpp"

int main(int argc, char** argv)
{	
	Mat image_source = charge_image(argc, argv);

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 10;

	// Séparation des pixels répartits dans les nb_seuils seuils.
	vector<Mat> ensembles_X = separe_en_seuils(image_source, NB_SEUILS); 

	// Calcul des contours
	vector<Mat> contours = calcul_contours(image_source, ensembles_X, NB_SEUILS);

	// Calcul des canaux hsv
	vector<Mat> canaux = calcul_S_et_O(contours, NB_SEUILS);
	Mat ensemble_O = canaux.at(0);
	Mat ensemble_S = canaux.at(2);
	
	// Calcul de l'image de synthèse
	Mat image_synthese;
	merge(canaux, image_synthese);
	cvtColor(image_synthese, image_synthese, COLOR_HSV2BGR);


	// On affiche les deux ensembles
	imshow("ensemble S", ensemble_S);
	imshow("ensemble O", ensemble_O);
	imshow("Synthese", image_synthese);

	int const PERIOD = 2;
	Mat reference_model[PERIOD];
	
	// On attend que la touche `q` ait été pressée
	while(waitKeyEx() != 113);
	return 0;
}
