#include "outils/outils.hpp"
#include "outils/constantes.hpp"
#include "detection/parallelogrammes.hpp"

int main(int argc, char** argv)
{	
	CommandLineParser parser(argc, argv, "{help h || Affiche ce message}{img-src || Image source}");
	if(parser.has("help"))
	{
		parser.printMessage();
	}

	string emplacement = parser.get<string>("img-src");
	Mat image_source = imread(emplacement, IMREAD_GRAYSCALE);

	imshow("Image source", image_source*255);
	return 0;
}
