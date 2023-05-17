#include "detection_contours/detection_contours.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, params);
	if(parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = parser.get<int>("nb-seuils");
	int const NB_ORIENTATIONS = parser.get<int>("nb-orientations");
	_calcul_table_orientations(NB_ORIENTATIONS);
	_calcul_table_normes();
	string emplacement = parser.get<String>("img-src");
	string mode = parser.get<string>("mode");

	Mat image_source;
	array<Mat, 2> SO;
	if(mode == "rgb")
	{
			image_source = chargement_image(emplacement);
			SO = calcul_SO_NB(image_source, NB_SEUILS, NB_ORIENTATIONS);
	}
	else if(mode == "ect" || mode == "tcs" || mode == "cts")
	{
		_calcul_tables_ect();
		image_source = chargement_image_hsv(emplacement);
		SO = calcul_SO_ECT(image_source, NB_SEUILS, NB_ORIENTATIONS);
	}
	else
	{
		cout << "Erreur: mode invalide (ce doit être \"ect\" ou \"rgb\")" << endl;
		return 1;
	}


	affiche_image_debug(SO.at(0), SO.at(1), NB_SEUILS, NB_ORIENTATIONS, "Contours");
	return 0;
}
