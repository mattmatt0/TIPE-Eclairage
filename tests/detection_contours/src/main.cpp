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

	Mat image_source = charge_image(argc, argv);

	// Nombre de seuils pour les lignes de niveaux
	int const NB_SEUILS = 2;
	int const NB_ORIENTATIONS = 200;


	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();
	array<Mat, 2> SO = calcule_SO_NB<NB_SEUILS>(image_source, NB_ORIENTATIONS);
	imshow("Orientations", SO.at(1)*20);
	imshow("S", SO.at(0)*20);

	/*Mat resultat = cree_image_orientations(OS.at(0)*100, OS.at(1), NB_SEUILS, NB_ORIENTATIONS);
	imshow("Resultat (RGB)", resultat);
	cvtColor(resultat, resultat, COLOR_HSV2BGR);
	imshow("Resultat (HSV)", resultat);*/

	while(waitKeyEx() != 113);
	return 0;
}
