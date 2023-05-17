#include "training/training.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

string const settings_images = "{rep-src | ../../../../images/test_ecm | Dossier contenant la séquence à analyser}{extension | png | Extension des images à rechercher}{nb-img | -1 | Nombre d'images à charger}";
string const settings_classifieur = "{model | ../../../images/trained1/cascade.xml | Fichier du modèle du classifieur}";

int main(int argc, char** argv)
{
	// On charge notre image
	CommandLineParser parser(argc, argv, settings_images+settings_classifieur);
	int nb_images = parser.get<int>("nb-img");
	string rep_source = parser.get<String>("rep-src");
	string extension = parser.get<String>("extension");
	string model = parser.get<String>("model");
	vector<Mat> images = chargement_repertoire_images(rep_source, extension, nb_images);	
	nb_images = images.size();

	// On déclare et charge notre classifieur	
	CascadeClassifier pietons_cascade;
	pietons_cascade.load(model);
	
	// Processus de détection
	vector<Rect> rectangles;

	for (int i = 0; i < nb_images; ++i)
	{
		pietons_cascade.detectMultiScale(images[i], rectangles);
	
		// Affichage sous forme d'encadrement des piétons trouvés dans l'image
		for (size_t j = 0; j < rectangles.size(); ++j)
		{
			rectangle(images[i], rectangles[j] , 255, 4);
		}
		cout << "Traitement de l'image " << i+1 << "/" << nb_images << endl;
	}

	for (int i = 0; i < nb_images; ++i)
	{
		imshow("resultat", images[i]);
		while (waitKeyEx() != TOUCHE_N);	
	}

	// On attend que la touche 'q' ait été préssée pour terminer le programme
	attend_q();
	return 0;
}
