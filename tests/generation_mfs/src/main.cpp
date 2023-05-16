#include "generation_mfs/generation_mfs.hpp"
#include "outils/constantes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, params_generiques+params_analyse+params_images+param_sauvegarde_dossier);
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
	string rep_dest = parser.get<String>("rep-dest");
	string extension = parser.get<String>("extension");

	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();

	vector<Mat> images = charge_repertoire_images(rep_source, extension, nb_images);
	vector<Mat> ensembles_O, ensembles_S;

	nb_images = images.size();
	for(int i = 0; i < nb_images; ++i)
	{
		cout << "Traitement de l'image n° " << i+1 << "/" << nb_images << endl;
		array<Mat, 2> SO = calcule_SO_NB(images.at(i), NB_SEUILS, NB_ORIENTATIONS);
		ensembles_S.push_back(SO.at(0));
		ensembles_O.push_back(SO.at(1));
	}


	int touche;
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{

		Mat D = calcule_D(ensembles_O, ensembles_S, t, T, NB_ORIENTATIONS, NB_SEUILS/2);
		Mat disp_imgO = cree_image_orientations(D*255, ensembles_O.at(t+T), NB_SEUILS, 1);
		imwrite(rep_dest+"/"+nombre_taille_fixe(t, 5)+"."+extension, disp_imgO);
		cout << rep_dest+"/"+nombre_taille_fixe(t, 5)+"."+extension << endl;
	}
	return 0;

}
