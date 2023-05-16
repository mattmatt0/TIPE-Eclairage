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
	int const NB_SEUILS = 50;
	int const NB_ORIENTATIONS = 200;


	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();
	array<Mat, 2> SO = calcule_SO_NB(image_source, NB_SEUILS, NB_ORIENTATIONS);

	Mat resultat = cree_image_orientations(SO.at(0), SO.at(1), NB_SEUILS, NB_ORIENTATIONS);
	imshow("Resultat (HSV)", resultat);

	while(waitKeyEx() != 113);
	return 0;
}
