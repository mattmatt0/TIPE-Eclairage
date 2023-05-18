#include "generation_ecm/generation_ecm.hpp"
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
	int const T = parser.get<int>("tau");
	int nb_images = parser.get<int>("nb-img");
	string rep_source = parser.get<String>("rep-src");
	string rep_dest = parser.get<String>("rep-dest");
	string extension = parser.get<String>("extension");

	_calcul_table_orientations(NB_ORIENTATIONS);
	_calcul_table_normes();
	_calcul_table_seuils(NB_SEUILS);

	vector<Mat> images = chargement_repertoire_images(rep_source, extension, nb_images);
	vector<Mat> ensembles_O, ensembles_S;
	nb_images = images.size();

	cout << "Calcul des contours..." << endl;
	for(int i = 0; i < nb_images; ++i)
	{
		array<Mat, 2> SO = calcul_SO_rapide(images.at(i), NB_SEUILS, NB_ORIENTATIONS);
		ensembles_S.push_back(SO.at(0));
		ensembles_O.push_back(SO.at(1));
		if((i & 7) == 7)
		{
			cout << i+1 << " images traitées sur " << nb_images << endl;
		}
	}
	cout << "Calcul des contours terminé !" << endl;

	int touche;
	cout << "Calcul de l'espace de caractéristiques de mouvement (ECM)..." << endl;
	cout << "Format de la forme: " << rep_dest+"/"+nombre_taille_fixe(0, 5)+"."+extension << endl;
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{

		Mat D = calcul_D(ensembles_O, ensembles_S, t, T, NB_ORIENTATIONS, NB_SEUILS/2);
		Mat disp_imgO = image_orientations(D*255, ensembles_O.at(t+T), NB_SEUILS, 1);
		imwrite(rep_dest+"/"+nombre_taille_fixe(t, 5)+"."+extension, disp_imgO);
	}
	cout << "Terminé ! " << endl;
	return 0;

}
