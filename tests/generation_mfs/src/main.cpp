#include "generation_mfs/generation_mfs.hpp"

int main(int argc, char** argv)
{	
	vector<Mat> images = charge_images("../../../../images/test_mfs", "png");
	vector<Mat> ensembles_O;
	int const NB_SEUILS = 10;
	int const NB_ORIENTATIONS = 90;
	int const T = 3;
	for(int i = 0; i < images.size(); ++i)
	{
		vector<Mat> ensembles_X = separe_en_seuils(images.at(i), NB_SEUILS);
		vector<Mat> contours = calcul_contours(images.at(i), ensembles_X, NB_SEUILS);
		vector<Mat> S_et_O = calcul_S_et_O(contours, NB_SEUILS);
		ensembles_O.push_back(S_et_O.at(0));
	}
	for(int t = 0; t + T < ensembles_O.size(); ++t)
	{
		Mat res = calcule_R(ensembles_O, t, T, NB_ORIENTATIONS);
		imshow("R(" + to_string(t) + ")", res);
		while(waitKeyEx() != 113);
	}
	return 0;
}
