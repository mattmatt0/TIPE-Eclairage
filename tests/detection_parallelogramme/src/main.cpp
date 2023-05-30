#include "outils/outils.hpp"
#include "detection_parallelogrammes/parallelogramme.hpp"
#include "outils/constantes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, "{help h || Affiche ce message}{img-src || Image source}");
	if(parser.has("help"))
	{
		parser.printMessage();
	}

	string emplacement = parser.get<string>("img-src");
	Mat image_source = imread(emplacement, IMREAD_GRAYSCALE);

	array<Mat, 2> resultat = calcule_bdf(image_source);
	cout << "Ow !" << endl;
	imshow("Image source", image_source*255);
	affiche_image_signee("BDF X", resultat.at(0));
	affiche_image_signee("BDF Y", resultat.at(1));
	return 0;
}
