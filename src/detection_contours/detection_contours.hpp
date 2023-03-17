#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <iostream>

void calcule_contours(cv::Mat &ensemble, cv::Mat &sortie)
{
	// Calcul des dérivées
	cv::Mat gradient_x, gradient_y;
	Sobel(ensemble, gradient_x, CV_32F, 1, 0, 3);
	Sobel(ensemble, gradient_y, CV_32F, 0, 1, 3);

	// Conversion en flottants parce qu'il le faut
	cv::normalize(gradient_x, gradient_x);
	cv::normalize(gradient_y, gradient_y);
	gradient_x *= 256.0;
	gradient_y *= 256.0;

	// Conversion (x,y) -> (norme, direction)
	cv::Mat direction;
	cv::Mat norme;
	cv::phase(gradient_x, gradient_y, direction, true); // "true" pour avoir des degrés	
	cv::magnitude(gradient_x, gradient_y, norme);

	// Décision des pixels qui font partie d'un contour ou non
	cv::Mat contour;
	cv::threshold(norme, contour, 0.5, 1.0, cv::THRESH_BINARY);

	// Ajout de l'orientation dans une matrice à trois canaux 
	// (le 1er représentant l'orientation, le 2e valant 1.0 tout le temps 
	//     et le 3e l'appartenance à un contour)
	std::vector<cv::Mat> canaux;
	canaux.push_back(direction);
	canaux.push_back(cv::Mat(direction.size(), CV_32F, 1.0));
	canaux.push_back(contour);
	cv::merge(canaux,sortie);
}
