#pragma once
#include <list>

vector<array<int, 4>> rectangles_englobant(Mat m_int, Mat m_orig)
{
	// Hyp : m_int est une image intégrale (d'une image binaire) de profondeur 32 bits signés
	// et m est l'image binaire d'origine
	Size taille = m_int.size();
	int tailleX = taille.width;
	int tailleY = taille.height;

	list<array<int, 3>> segments;
	vector<array<int, 4>> res;
	int deb_seg;
	int fin_seg;
	int deb_zone;
	int fin_zone;
	
	for(int x = 0; x < tailleX; ++x)
	{
		if(!segments.empty())
		{
			printf("========== LISTE DES SEGMENTS À x=%d ===============\n", x);
			for(auto const& segment : segments)
				printf("%d -> [%d, %d]\n", segment.at(2), segment.at(0), segment.at(1));
		}



		// Essaie de prolonger les segments déjà trouvés
		auto segment = segments.begin();
		while(segment != segments.end())
		{
			deb_seg = (*segment).at(0) - 1; 
			fin_seg = (*segment).at(1) + 1;
			// Prolonge le segment ...
			// ... en haut
			if(segment != segments.begin())
				deb_zone = (*prev(segment)).at(1);
			else deb_zone = 0;

			printf("Aire(%d, %d, %d, %d) = %d\n", (*segment).at(2), deb_seg, x, deb_seg, aire(m_int, (*segment).at(2), deb_seg, x, deb_seg+1));
			while(aire(m_int, (*segment).at(2), deb_seg, x, deb_seg) != 0 && --deb_seg >= deb_zone);
			(*segment).at(0) = deb_seg+1;
			
			// ... puis en bas
			if(next(segment) != segments.end())
				fin_zone = (*next(segment)).at(0);
			else fin_zone = tailleY - 1;

			while(aire(m_int, (*segment).at(2), fin_seg, x, fin_seg) != 0 && ++fin_seg <= fin_zone);
			(*segment).at(1) = fin_seg - 1;

			++segment;
		}


		if(!segments.empty())
		{
			printf(">== LISTE DES SEGMENTS À x=%d après prolongation ==<\n", x);
			for(auto const& segment : segments)
				printf("%d -> [%d, %d]\n", segment.at(2), segment.at(0), segment.at(1));
		}

		// Fusionne les segments
		for(segment = segments.begin(); segment != segments.end(); ++segment)
		{
			while(next(segment) != segments.end())
			{
				fin_seg = (*segment).at(1);
				deb_seg = (*next(segment)).at(0);
				if(fin_seg >= deb_seg)
				{
					(*segment).at(0) = min((*next(segment)).at(0), (*segment).at(0));
					(*segment).at(1) = max((*next(segment)).at(1), (*segment).at(1));
					(*segment).at(2) = min((*next(segment)).at(2), (*segment).at(2));
					segments.erase(next(segment));
				}
				else break;
			}
		}


		if(!segments.empty())
		{
			printf(">== LISTE DES SEGMENTS À x=%d après fusion ==<\n", x);
			for(auto const& segment : segments)
				printf("%d -> [%d, %d]\n", segment.at(2), segment.at(0), segment.at(1));
		}

		// Ferme les segments vides
		for(segment = segments.begin(); segment != segments.end();) 
		{
			// Vérifie si le bout est non vide
			deb_seg = (*segment).at(0);
			fin_seg = (*segment).at(1);
			if(aire(m_int, x, deb_seg, x, fin_seg) == 0)
			{
				// Fin de zone détectée: on renvoie alors le rectangle correspondant
				res.push_back({(*segment).at(2), deb_seg, x-1, fin_seg});
				segment = segments.erase(segment);
			}
			else segment++;
		}

		if(!segments.empty())
		{
			printf(">== LISTE DES SEGMENTS À x=%d après fermeture ==<\n", x);
			for(auto const& segment : segments)
				printf("%d -> [%d, %d]\n", segment.at(2), segment.at(0), segment.at(1));
		}


		// Ajoute ensuite les autres pixels
		bool en_segment = false;

			
		deb_zone = 0;
		for(auto segment = segments.begin(); segment != segments.end(); ++segment)
		{
			fin_zone = (*segment).at(0) - 1;
			for(int i = deb_zone; i <= fin_zone; ++i)
			{
				if(x == 322) printf("i=%d\n", i);
				if(m_orig.at<uint8_t>(i,x) == 0 && en_segment)
				{
					en_segment = false;
					segments.insert(segment, {deb_seg, fin_seg, x});
				}
				if(m_orig.at<uint8_t>(i,x) == 1 && en_segment)
					fin_seg++;
				if(m_orig.at<uint8_t>(i,x) == 1 && !en_segment)
				{
					deb_seg = i;
					fin_seg = deb_seg;
					en_segment = true;
				}
			}
			deb_zone = (*segment).at(1) + 1;
		}
		fin_zone = tailleY - 1;
		for(int i = deb_zone; i < fin_zone; ++i)
		{
			if(m_orig.at<uint8_t>(i,x) == 0 && en_segment)
			{
				en_segment = false;
				segments.push_back({deb_seg, fin_seg, x});
			}
			if(m_orig.at<uint8_t>(i,x) == 1 && en_segment)
				fin_seg++;
			if(m_orig.at<uint8_t>(i,x) == 1 && !en_segment)
			{
				deb_seg = i;
				fin_seg = deb_seg;
				en_segment = true;
			}
		}
	}
	
	for(auto const& segment : segments)
		res.push_back({segment.at(2), segment.at(0), tailleX-1, segment.at(1)});
	return res;
}

