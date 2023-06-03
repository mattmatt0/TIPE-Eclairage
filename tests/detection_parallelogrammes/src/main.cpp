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
	Mat integrale = precalcul_integral(image_source);
	Mat dessin = image_source.clone() * 255;
	Mat integrale_disp;
	integrale.convertTo(integrale_disp, CV_32FC1);
	normalize(integrale_disp, integrale_disp);
	imshow("Image integrale", integrale_disp*255);


	for(auto const& rect : rectangles_englobant(integrale, image_source))
	{
		printf("%d %d %d %d\n", rect.at(0), rect.at(1), rect.at(2), rect.at(3));
		rectangle(dessin, Point(rect.at(0),rect.at(1)), Point(rect.at(2), rect.at(3)), 255, 4);
	}

	imshow("Image source", image_source*255);
	imshow("Resultat", dessin);
	attend_q();
	return 0;
}
