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
	std::cout << "Calcul pour " << NB_ORIENTATIONS << "orientations" << endl;
	int const T = parser.get<int>("tau");
	int nb_images = parser.get<int>("nb-img");
	string rep_source = parser.get<String>("rep-source");
	string extension = parser.get<String>("extension");

	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();

	vector<Mat> images = charge_repertoire_images(rep_source, extension, nb_images);
	vector<Mat> ensembles_O;

	nb_images = images.size();
	for(int i = 0; i < nb_images; ++i)
	{
		cout << "Traitement de l'image n° " << i+1 << "/" << nb_images << endl;
		array<Mat, 2> SO = calcule_SO_NB(images.at(i), NB_SEUILS, NB_ORIENTATIONS);
		ensembles_O.push_back(SO.at(1));
	}


	int touche;
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{

		Mat D = calcule_D(ensembles_O, t, T, NB_ORIENTATIONS);
		imshow("D", D);
		Mat disp_imgO = cree_image_orientations(D*255, ensembles_O.at(t+T), NB_SEUILS, NB_ORIENTATIONS);
		imshow("O", disp_imgO);
		while(touche = waitKeyEx())
		{
			if(touche == TOUCHE_Q) {t = ensembles_O.size(); break;}
			else if (touche == TOUCHE_N) break;
		};
	}
	return 0;

}
