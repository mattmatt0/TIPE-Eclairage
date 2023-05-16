#pragma once
string const params_generiques = "{help h || Affiche ce message}";
string const params_analyse = "{nb-seuils | 10 | Nombre de seuils à étudier}{nb-orientations | 10 | Nombre d'orientations possibles}{tau | 2 | Temps de stabilité (valeur de T)}";
string const params_images = "{rep-source | ../../../../images/test_mfs | Dossier contenant la séquence à analyser}{extension | png | Extension des images à rechercher}{nb-img | -1 | Nombre d'images à charger}";
string const param_sauvegarde_dossier = "{rep-dest | ../../../../images/resultats | Dossier contenant les résultats}";

unsigned char TOUCHE_N = 110;
unsigned char TOUCHE_Q = 113;

float PI = 3.141592653589793238462643483279502884197169399365105820974944592307816406;
float RAD_PAR_OCT = 2*PI/256;

