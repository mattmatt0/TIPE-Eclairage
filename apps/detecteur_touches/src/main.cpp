#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{	
	namedWindow("Afficheur de touches");
	int n;
	while(true)
	{
		if(( n = waitKeyEx() ) != -1)
		{
			cout << n << endl;
		}
	}
	destroyWindow("Afficheur de touches");
	return 0;
}
