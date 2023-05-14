#include "generation_mfs/generation_mfs.hpp"
#include "outils/constantes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, params_generiques+params_analyse+params_images);
	if(parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	int const NB_SEUILS = parser.get<int>("nb-seuils");
	int const NB_ORIENTATIONS = parser.get<int>("nb-orientations");
	int const T = parser.get<int>("tau");
	int nb_images = parser.get<int>("nb-img");
	string rep_source = parser.get<String>("rep-source");
	string extension = parser.get<String>("extension");

	vector<Mat> images = charge_repertoire_images(rep_source, extension, nb_images);
	vector<Mat> ensembles_O;

	nb_images = images.size();
	for(int i = 0; i < nb_images; ++i)
	{
		cout << "Traitement de l'image n° " << i+1 << "/" << nb_images << endl;
		vector<Mat> ensembles_X = separe_en_seuils(images.at(i), NB_SEUILS);
		vector<Mat> contours = calcul_contours(images.at(i), ensembles_X, NB_SEUILS);
		vector<Mat> S_et_O = calcul_S_et_O(contours, NB_SEUILS);
		ensembles_O.push_back(S_et_O.at(0));
	}

	int touche;
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{

		Mat D = calcule_D(ensembles_O, t, T, NB_ORIENTATIONS);
		Mat disp_img = cree_image_orientations(D);
		imshow("D", disp_img);
		Mat O_entier;
		ensembles_O.at(t+T).convertTo(O_entier, CV_8UC1);
		Mat disp_imgO = cree_image_orientations(O_entier);
		imshow("O", disp_imgO);
		while(touche = waitKeyEx())
		{
			if(touche == TOUCHE_Q) {t = ensembles_O.size(); break;}
			else if (touche == TOUCHE_N) break;
		};
	}
	return 0;
}
