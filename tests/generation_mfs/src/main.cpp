#include "generation_mfs/generation_mfs.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, "{@source_dir | ../../../../images/test_mfs | Dossier contenant la séquence à analyser}{@extension | png | Extension des images à rechercher}");
	vector<Mat> images = charge_repertoire_images(parser.get<String>("@source_dir"), parser.get<String>("@extension"));
	vector<Mat> ensembles_O;
	int const NB_SEUILS = 10;
	int const NB_ORIENTATIONS = 10;
	int const T = 2;
	for(int i = 0; i < images.size(); ++i)
	{
		cout << "Traitement de l'image n° " << i+1 << "/" << images.size() << endl;
		vector<Mat> ensembles_X = separe_en_seuils(images.at(i), NB_SEUILS);
		vector<Mat> contours = calcul_contours(images.at(i), ensembles_X, NB_SEUILS);
		vector<Mat> S_et_O = calcul_S_et_O(contours, NB_SEUILS);
		ensembles_O.push_back(S_et_O.at(0));
	}
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{

		cout << "Calcul de D entre t = " << t << " et t = " << t+T << endl;
		Mat D = calcule_D(ensembles_O, t, T, NB_ORIENTATIONS);
		Mat disp_img = cree_image_orientations(D);
		imshow("D", disp_img);
		Mat O_entier;
		ensembles_O.at(t+T).convertTo(O_entier, CV_8UC1);
		Mat disp_imgO = cree_image_orientations(O_entier);
		imshow("O", disp_imgO);
		while(waitKeyEx() != 113);
	}
	return 0;
}
