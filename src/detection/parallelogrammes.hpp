#pragma once
#include <list>

vector<array<int, 4>> rectangles_englobant(Mat m_int, Mat m_orig)
{
	// Hyp : m_int est une image intégrale (d'une image binaire) de profondeur 32 bits non signés
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
	int aire;
	
	for(int x = 0; x < tailleX; ++x)
	{
		// Essaie de prolonger les segments déjà trouvés
		for(auto segment = segments.begin(); segment != segments.end(); ++segment)
		{
			// Vérifie si le segment est non vide
			deb_seg = (*segment).at(0);
			fin_seg = (*segment).at(1);
			if(deb_seg > 0)
				aire = m_int.at<uint32_t>(fin_seg,x) - m_int.at<uint32_t>(deb_seg-1,x);
			else aire = m_int.at<uint32_t>(fin_seg, x);
			if(aire == 0)
			{
				// Fin de zone détectée: on renvoie alors le rectangle correspondant
				res.push_back({(*segment).at(2), deb_seg, x-1, fin_seg});
				segments.erase(segment);
			}
			else
			{
				// Prolonge le segment ...
				// ... en haut
				if(segment != segments.begin())
				{
					deb_zone = (*prev(segment)).at(1);
					deb_seg--;
					do aire = m_int.at<uint32_t>(deb_seg,x) - m_int.at<uint32_t>(deb_seg,(*segment).at(2)-1);
					while(aire != 0 && --deb_seg >= deb_zone);
					(*segment).at(0) = deb_seg+1;
				}
				
				// ... puis en bas
				if(next(segment) != segments.end())
				{
					fin_zone = (*next(segment)).at(0);
					fin_seg++;
					do aire = m_int.at<uint32_t>(fin_seg,x) - m_int.at<uint32_t>(fin_seg,(*segment).at(2)-1);
					while(aire != 0 && ++fin_seg <= fin_zone);
					(*segment).at(1) = fin_seg;

				}
				
			}
		}

		// Fusionne les segments
		for(auto segment = segments.begin(); segment != segments.end(); ++segment)
		{
			while(next(segment) != segments.end())
			{
				fin_seg = (*segment).at(1);
				deb_seg = (*next(segment)).at(0);
				if(fin_seg >= deb_seg)
				{
					(*segment).at(0) = min((*next(segment)).at(0), (*segment).at(0));
					(*segment).at(1) = max((*next(segment)).at(1), (*segment).at(1));
					segments.erase(next(segment));
				}
				else break;
			}
		}

		// Ajoute ensuite les autres pixels
		for(auto segment = segments.begin(); segment != segments.end(); ++segment)
		{
			if(segment == segments.begin())
				deb_zone = 0;
			else deb_zone = (*prev(segment)).at(1) + 1;
			if(next(segment) == segments.end())
				fin_zone = tailleY - 1;
			else fin_zone = (*next(segment)).at(0) - 1;
			bool en_segment = false;
			for(int i = deb_zone; i <= fin_zone; ++i)
			{
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
		}
	}
	return res;
}

