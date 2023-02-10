#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char** argv)
{
	CommandLineParser parser(argc, argv, "{@image | ../../../img.png | Image sur laquelle on va opérer nos transformations}");
	Mat image_source = imread(samples::findFile(parser.get<String>( "@image" )),IMREAD_COLOR);
	namedWindow("image", WINDOW_AUTOSIZE);
	imshow("image", image_source);
	while(waitKeyEx() != 113);
	return 0;
}
