#include "HoughTransform/HoughTransform.hpp"

int main(int argc, char** argv)
{	
	// Chargement de l'image, conversion en noir et blanc et ajout d'un faible flou gaussien
	Mat lines_gray = charge_image(argc, argv);
	//imshow("lines_gray", lines_gray);
	
	// Detection des contours
	Mat edges = canny(lines_gray, 50, 100);
	//imshow("edges", edges);

	//setMouseCallback("edges", coordonnees_image);	
	
	// Définition de la région d'intérêt
	Mat roi = region_interet(edges);
	//imshow("roi", roi);

	// Transformée de Hough
	Mat HoughTransformed = roi;
	Mat colorHoughTransformed = transformee_hough(HoughTransformed);
	imshow("colorHoughTransformed", colorHoughTransformed);

	// On attend que la touche `q` ait été pressée
	while(waitKeyEx() != 113);
	return 0;
}
