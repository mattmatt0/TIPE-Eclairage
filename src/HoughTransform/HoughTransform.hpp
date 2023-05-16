#include "outils/outils.hpp"

Mat canny(Mat image_input, double threshold1, double threshold2)
{
	Mat edges;
	Canny(image_input, edges, threshold1, threshold2);
	return edges;
}

void coordonnees_image(int event, int x, int y, int flags, void* userdata)
{
	//Permet d'obtenir les coordonnées d'un point d'une image affichée en cliquant sur un point
	if(event == EVENT_LBUTTONDOWN )
		printf("(%d, %d)\n", x, y);	
}

Mat region_interet(Mat image_input)
{
	// Défini la région d'intérêt grâce à un masque appliqué sur image_input
	Mat masque = Mat::zeros(image_input.rows, image_input.cols, CV_8U);
	Point polygone[1][5];
	polygone[0][0] = Point(0, 534);
	polygone[0][1] = Point(501, 302);
	polygone[0][2] = Point(600, 293);
	polygone[0][3] = Point(1014, 699);
	polygone[0][4] = Point(2, 701);
	polygone[0][5] = Point(0, 534);
	const Point* points[1] = {polygone[0]};
	int nombre_points[1] = {6};

	fillPoly(masque, points, nombre_points, 1, 255, LINE_8);
	Mat roi;
	bitwise_and(image_input, image_input, roi, masque);
	return roi;
}

void dessine_lignes(Mat image_input, vector<Vec4i> lignes)
{
	// Affiche les lignes de la transformée de Hough
	for (size_t i = 0; i < lignes.size(); i++)
	{	
		Point pt1, pt2;
		pt1.x = lignes[i][0];
		pt1.y = lignes[i][1];
		pt2.x = lignes[i][2];
		pt2.y = lignes[i][3];
		line(image_input, pt1, pt2, Scalar(0, 0, 255), 3, 10);
	}
}

Mat transformee_hough(Mat input_image)
{
	// Renvoie l'image avec les lignes détectées après la transformée de Hough
	Mat colorHoughTransformed;
	cvtColor(input_image, colorHoughTransformed, COLOR_GRAY2BGR);
	vector<Vec4i> lignes;
	HoughLinesP(input_image, lignes, 1, CV_PI/180, 0, 0, 0);
	dessine_lignes(colorHoughTransformed, lignes);
	return colorHoughTransformed;
}
