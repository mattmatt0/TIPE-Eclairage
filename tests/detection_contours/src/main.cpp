#include "detection_contours/detection_contours.hpp"
#include "outils/constantes.hpp"

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
	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();
	string emplacement = parser.get<String>("img-src");
	string mode = parser.get<string>("mode");

	Mat image_source;
	array<Mat, 2> SO;
	if(mode == "rgb")
	{
			image_source = charge_image(emplacement);
			SO = calcule_SO_NB(image_source, NB_SEUILS, NB_ORIENTATIONS);
	}
	else if(mode == "ect" || mode == "tcs" || mode == "cts")
	{
		_calcule_tables_ect();
		image_source = charge_image_hsv(emplacement);
		SO = calcule_SO_ECT(image_source, NB_SEUILS, NB_ORIENTATIONS);
	}
	else
	{
		cout << "Erreur: mode invalide (ce doit être \"ect\" ou \"rgb\")" << endl;
		return 1;
	}

	Mat resultat = cree_image_orientations(SO.at(0), SO.at(1), NB_SEUILS, NB_ORIENTATIONS);
	imshow("Resultat", resultat);

	while(waitKeyEx() != 113);
	return 0;
}
