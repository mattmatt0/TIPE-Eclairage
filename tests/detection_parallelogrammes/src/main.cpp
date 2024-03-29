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
	Mat dessin;

	Mat integrale = image_integrale(image_source);
	Mat integrale_disp;
	integrale.convertTo(integrale_disp, CV_32FC1);
	normalize(integrale_disp, integrale_disp);
	imshow("Image integrale", integrale_disp*255);

	cvtColor(image_source*255, dessin,COLOR_GRAY2RGB);


	vector<array<int, 4>> rectangles =  rectangles_englobant(integrale, image_source);
	printf("==============================================\n");
	for(auto const& rect : rectangles)
	{
		printf("%d %d %d %d\n", rect.at(0), rect.at(1), rect.at(2), rect.at(3));
		rectangle(dessin, Point(rect.at(0),rect.at(1)), Point(rect.at(2), rect.at(3)), Scalar(0,0,255), 1);
	}

	imshow("Image source", image_source*255);
	imshow("Resultat", dessin);
	attend_q();
	return 0;
}
