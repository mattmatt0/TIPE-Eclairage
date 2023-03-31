#include "detection_contours/detection_contours.hpp"

int main(int argc, char** argv)
{	
	Mat image_source = charge_image_hsv(argc, argv);
	imshow("img_source", image_source);
	struct TroisCanaux image_source_splited = hsv2SplitedChannels(image_source);
	/*imshow("img_hsv_1", image_source_splited.canal1);
	imshow("img_hsv_2", image_source_splited.canal2);
	imshow("img_hsv_3", image_source_splited.canal3);*/
	struct TroisCanaux TCS_img = calcul_TCS(image_source_splited);	
	imshow("img1", TCS_img.canal1);
	imshow("img2", TCS_img.canal2);
	imshow("img3", TCS_img.canal3);
	

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 10;
/*
	// Séparation des pixels répartits dans les nb_seuils seuils.
	vector<Mat> ensembles_X1 = separe_en_seuils(TCS_img.canal1, NB_SEUILS);
	vector<Mat> ensembles_X2 = separe_en_seuils(TCS_img.canal2, NB_SEUILS);
	vector<Mat> ensembles_X3 = separe_en_seuils(TCS_img.canal3, NB_SEUILS); 

	// Calcul des contours
	vector<Mat> contours1 = calcul_contours(TCS_img.canal1, ensembles_X1, NB_SEUILS);
	vector<Mat> contours2 = calcul_contours(TCS_img.canal2, ensembles_X2, NB_SEUILS);
	vector<Mat> contours3 = calcul_contours(TCS_img.canal3, ensembles_X3, NB_SEUILS);

	// Calcul des canaux hsv
	vector<Mat> canaux1 = calcul_S_et_O(contours1, NB_SEUILS);
	vector<Mat> canaux2 = calcul_S_et_O(contours2, NB_SEUILS);
	vector<Mat> canaux3 = calcul_S_et_O(contours3, NB_SEUILS);
	Mat ensemble_O1 = canaux1.at(0);
	Mat ensemble_S1 = canaux1.at(2);
	Mat ensemble_O2 = canaux2.at(0);
	Mat ensemble_S2 = canaux2.at(2);
	Mat ensemble_O3 = canaux3.at(0);
	Mat ensemble_S3 = canaux3.at(2);
	
	// Calcul de l'image de synthèse
	Mat image_synthese1;
	merge(canaux1, image_synthese1);
	cvtColor(image_synthese1, image_synthese1, COLOR_HSV2BGR);


	// On affiche les deux ensembles
	imshow("ensemble S", ensemble_S1);
	imshow("ensemble O", ensemble_O1);
	imshow("Synthese", image_synthese1);

	int const PERIOD = 9;
	Mat reference_model[PERIOD];
	*/
	
	// On attend que la touche `q` ait été pressée
	while(waitKeyEx() != 113);
	return 0;
}
