#include "generation_ecm/generation_ecm.hpp"
#include "outils/constantes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, params_generiques+params_analyse+params_images+param_sauvegarde_dossier+"{affiche-progres a || Affiche la progression du traitement}{mode-res | nb | Indiquer 'nb' pour sauvegarder uniquement la valeur de D, 'couleur' pour indiquer aussi l'orientation associée}");
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
	bool progression = parser.has("affiche-progres");
	bool ajoute_orientation = (parser.get<String>("mode-res") == "couleur");

	_calcul_table_orientations(NB_ORIENTATIONS);
	_calcul_table_normes();
	_calcul_table_seuils_moderne(NB_SEUILS);

	vector<string> *noms_fichiers = new vector<string>;
	vector<Mat> images = chargement_repertoire_images(rep_source, extension, nb_images, noms_fichiers);
	vector<Mat> ensembles_O, ensembles_S;
	nb_images = images.size();

	if(progression)
	{
		cout << "Calcul des contours..." << endl;
		for(int i = 0; i < nb_images; ++i)
		{
			array<Mat, 2> SO = calcul_SO_rapide(images.at(i), NB_SEUILS, NB_ORIENTATIONS);
			ensembles_S.push_back(SO.at(0));
			ensembles_O.push_back(SO.at(1));
			cout << i+1 << " images traitées sur " << nb_images << endl;
		}
		cout << "Calcul des contours terminé !" << endl;
		cout << "Calcul de l'espace de caractéristiques de mouvement (ECM)..." << endl;
		cout << "Format de la forme: " << rep_dest+"/"+nombre_taille_fixe(0, 5)+"."+extension << endl;
	}
	else
	{
		for(int i = 0; i < nb_images; ++i)
		{
			array<Mat, 2> SO = calcul_SO_rapide(images.at(i), NB_SEUILS, NB_ORIENTATIONS);
			ensembles_S.push_back(SO.at(0));
			ensembles_O.push_back(SO.at(1));
		}
	}

	int touche;
	
	for(int t = T; t < ensembles_O.size(); ++t)
	{
		Mat D = calcul_D(ensembles_O, ensembles_S, t, T, NB_ORIENTATIONS, NB_SEUILS/8);
		if(ajoute_orientation)
		{
			Mat disp_img = image_orientations(D*255, ensembles_O.at(t), NB_SEUILS, NB_ORIENTATIONS);
			imwrite(rep_dest+"/"+noms_fichiers->at(t), disp_img);
		}
		else imwrite(rep_dest+"/"+noms_fichiers->at(t), D*255);
	}
	if(progression) cout << "Terminé ! " << endl;
	return 0;

}
