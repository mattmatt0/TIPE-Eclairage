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
	int const NB_SEUILS = 10;
	int const NB_ORIENTATIONS = 20;


	_calcule_orientations(NB_ORIENTATIONS);
	_calcule_normes();
	array<Mat, 2> OS = calcule_OS_NB<NB_SEUILS>(image_source, NB_ORIENTATIONS);
	imshow("", cree_image_orientations(MFS));

	while(waitKeyEx() != 113);
	return 0;
}
