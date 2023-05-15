#include "detection_contours/detection_contours.hpp"
#include "outils/constantes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, "{img-source | ../../../../images/lenna.jpg | Image sur laquelle on va calculer effectuer nos opérations}{help h || Affiche ce message}{mode | tcs | Espace colorimétrique à utiliser: soit tcs (Texton Color Space), soit rgb}");
	if(parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	Mat image_source = charge_image_hsv(argc, argv);
	array<Mat, 3> image_source_splited = separe_hsv(image_source);
	array<Mat, 3> TCS_img = calcul_TCS(image_source_splited);	
	imshow("img1", TCS_img.at(0));
	

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 10;

	// Séparation des pixels répartis dans les NB_SEUILS seuils.
	array<Mat, NB_SEUILS> ensembles_X1 = separe_en_seuils<NB_SEUILS>(TCS_img.at(0));
	array<Mat, NB_SEUILS> ensembles_X2 = separe_en_seuils<NB_SEUILS>(TCS_img.at(1));
	array<Mat, NB_SEUILS> ensembles_X3 = separe_en_seuils<NB_SEUILS>(TCS_img.at(2)); 

	// Calcul des contours
	array<Mat, NB_SEUILS> contours1 = calcul_contours<NB_SEUILS>(ensembles_X1);
	array<Mat, NB_SEUILS> contours2 = calcul_contours<NB_SEUILS>(ensembles_X2);
	array<Mat, NB_SEUILS> contours3 = calcul_contours<NB_SEUILS>(ensembles_X3);

	// Calcul des canaux hsv
	array<Mat, 2> canaux1 = calcul_S_et_O<NB_SEUILS>(contours1);
	array<Mat, 2> canaux2 = calcul_S_et_O<NB_SEUILS>(contours2);
	array<Mat, 2> canaux3 = calcul_S_et_O<NB_SEUILS>(contours3);
	Mat ensemble_O1 = canaux1.at(0);
	Mat ensemble_S1 = canaux1.at(1);
	Mat ensemble_O2 = canaux2.at(0);
	Mat ensemble_S2 = canaux2.at(1);
	Mat ensemble_O3 = canaux3.at(0);
	Mat ensemble_S3 = canaux3.at(1);

	array<Mat, 2> synth1;
	array<Mat, 2> synth2;
	array<Mat, 2> synth3;

	synth1 = {ensemble_O1 ,ensemble_S1};
	synth2 = {ensemble_O2 ,ensemble_S2};
	synth3 = {ensemble_O3 ,ensemble_S3};

	array<array<Mat, 2>, 3> datas = {synth1, synth2, synth3};

	
	
	// Calcul de l'image de synthèse
	array<Mat, 3> mfs_cts = synthese_S_O_CTS(datas);

	Mat img_mfs_cts;
	merge(mfs_cts, img_mfs_cts);
	cvtColor(img_mfs_cts, img_mfs_cts, COLOR_HSV2BGR);

	imshow("MFS CTS", img_mfs_cts);
	
	// On attend que la touche `q` ait été pressée
	while(waitKeyEx() != 113);
	return 0;
}
