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
	_calcul_table_seuils(NB_SEUILS);
	string emplacement = parser.get<String>("img-src");
	string mode = parser.get<string>("mode");

	Mat image_source;
	Mat image_source1;
	Mat image_source2;
	array<Mat, 2> SO1;
	array<Mat, 2> SO2;
	
	if(mode == "rgb")
	{
			image_source1 = chargement_image(emplacement);
			image_source2 = chargement_image(emplacement);
			SO1 = calcul_SO_rapide(image_source1, NB_SEUILS, NB_ORIENTATIONS);
			cout << "Fin du calcul rapide" << endl;
			SO2 = calcul_SO_NB(image_source2, NB_SEUILS, NB_ORIENTATIONS);
			cout << "Fin du calcul classique" << endl;
	}
	else if(mode == "ect" || mode == "tcs" || mode == "cts")
	{
		_calcul_tables_ect();
		image_source = chargement_image_hsv(emplacement);
		SO1 = calcul_SO_ECT(image_source, NB_SEUILS, NB_ORIENTATIONS);
	}
	else
	{
		cout << "Erreur: mode invalide (ce doit être \"ect\" ou \"rgb\")" << endl;
		return 1;
	}
	
	affiche_image_debug(SO1.at(0), SO1.at(1), NB_SEUILS, NB_ORIENTATIONS, "Contours (Rapide)");
	affiche_image_debug(SO2.at(0), SO2.at(1), NB_SEUILS, NB_ORIENTATIONS, "Contours (Ancien)");
	attend_q();
	return 0;
}
