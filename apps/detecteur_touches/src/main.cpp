#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{	
	/*
	Mat image_source = charge_image_hsv(argc, argv);
	//imshow("img_source", image_source);
	struct TroisCanaux image_source_splited = hsv2SplitedChannels(image_source);
	imshow("img_hsv_1", image_source_splited.canal1);
	imshow("img_hsv_2", image_source_splited.canal2);
	imshow("img_hsv_3", image_source_splited.canal3);
	struct TroisCanaux TCS_img = calcul_TCS(image_source_splited);	
	imshow("img1", TCS_img.canal1);
	//imshow("img2", TCS_img.canal2);
	//imshow("img3", TCS_img.canal3);
	

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 10;

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

	vector<Mat> synth1;
	vector<Mat> synth2;
	vector<Mat> synth3;

	synth1.push_back(ensemble_O1);
	synth1.push_back(ensemble_S1);
	synth2.push_back(ensemble_O2);
	synth2.push_back(ensemble_S2);
	synth3.push_back(ensemble_O3);
	synth3.push_back(ensemble_S3);
	vector<vector<Mat>> datas;
	datas.push_back(synth1);
	datas.push_back(synth2);
	datas.push_back(synth3);

	
	
	// Calcul de l'image de synthèse
	vector<Mat> mfs_cts = synthese_S_O_CTS(datas);

	
	Mat image_synthese1;
	merge(canaux1, image_synthese1);
	cvtColor(image_synthese1, image_synthese1, COLOR_HSV2BGR);

	Mat image_synthese2;
	merge(canaux2, image_synthese2);
	cvtColor(image_synthese2, image_synthese2, COLOR_HSV2BGR);

	Mat image_synthese3;
	merge(canaux3, image_synthese3);
	cvtColor(image_synthese3, image_synthese3, COLOR_HSV2BGR);


	// On affiche les deux ensembles
	imshow("ensemble S1", ensemble_S1);
	imshow("ensemble O1", ensemble_O1);
	imshow("Synthese1", image_synthese1);
	imshow("ensemble S2", ensemble_S2);
	imshow("ensemble O2", ensemble_O2);
	imshow("Synthese2", image_synthese2);
	imshow("ensemble S3", ensemble_S3);
	imshow("ensemble O3", ensemble_O3);
	imshow("Synthese3", image_synthese3);

	int const PERIOD = 9;
	Mat reference_model[PERIOD];
	

	Mat img_mfs_cts;
	merge(mfs_cts, img_mfs_cts);
	cvtColor(img_mfs_cts, img_mfs_cts, COLOR_HSV2BGR);

	imshow("MFS CTS", img_mfs_cts);
	
	// On attend que la touche `q` ait été pressée
	while(waitKeyEx() != 113);*/
	Mat image_source = imread("../../../../images/lenna.png", IMREAD_GRAYSCALE);
	imshow("img_source", image_source);
	int n;
	while(true)
	{
		if(( n = waitKeyEx() ) != -1)
		{
			cout << n << endl;
		}
	}
	return 0;
}
