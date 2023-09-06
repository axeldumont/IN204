// Ceci est le code principale du raytracer.
// Il contient : la fonction d'itération et de calcul du raytracing "draw"

// Pour la conversion tga->png
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../hpp/conversion_tga_png/stb_image.hpp"
#include "../hpp/conversion_tga_png/stb_image_write.hpp"

// Librairies essentielles
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "../hpp/tga_image.hpp"

// Pour paralléliser
#include <omp.h>

// Pour la lisibilité
using namespace std;

/*************** CALCUL DU RAY TRACING ET CREATION DE L'IMAGE ***************/
bool draw(char *outputName, scene &myScene)
{

  // Création du fichier output en binaire
  ofstream imageFile(outputName, ios_base::binary);

  // Header TGA
  if (!header_tga(outputName, myScene, imageFile))
  {
    std::cout << "Problème d'image" << endl;
    return false;
  }
  // Code pour la barre de chargement
  cout << "Image n°" << outputName[12];
  cout << "              [";
  // Balayage des rayons lumineux
  for (int y = 0; y < myScene.sizey; ++y)
  { // On parcourt tous les pixels de l'image
    if (y % (myScene.sizey / 20) == 0)
    {
      cout << "#" << flush;
    }
    for (int x = 0; x < myScene.sizex; ++x)
    {
      color output = {0.0f, 0.0f, 0.0f}; 
      for (float fragx = float(x); fragx < x + 1.0f; fragx += 0.5f)
      {
        for (float fragy = float(y); fragy < y + 1.0f; fragy += 0.5f)
        {
          float coef = 1.0f;
          int level = 0;
          float sampleRatio = 0.25f; // Antialiasing x4
          // Lancer de rayon
          ray viewRay = {{fragx, fragy, -10000.0f}, {0.0f, 0.0f, 1.0f}}; // Le 1er rayon est "perprendiculaire" à l'écran et commence a -10 000
                                                                               // Ce premier rayon est "virtuel" et sert surtout a quel objet appartient le pixel qu'on parcourt, et la normale de l'objet en ce point

          // Boucle while qui s'arrête après x itérations ou si on a une reflection négative
          do
          {
            // recherche de l'intersection la plus proche
            float t = 30000.0f; // On prend comme distance "infinie" 30 000
            int currentObject = -1;
            string currentObjectType = "None";
            vecteur n;
            for (unsigned int i = 0; i < myScene.objTab.size(); ++i) // Pour chacun des objets
            {
              if (myScene.objTab[i].type == "sphere")
              {
                if (hitSphere(viewRay, myScene.objTab[i], t))
                { // Si l'objet intersecte le rayon
                  currentObject = i;
                  currentObjectType = "sphere"; // On le prend comme l'objet 'actuel'
                }
              }
              if (myScene.objTab[i].type == "cube")
              {
                if (hitCube(viewRay, myScene.objTab[i], t, n))
                { // Si l'objet intersecte le rayon
                  currentObject = i;
                  currentObjectType = "cube"; // On le prend comme l'objet 'actuel'
                }
              }

              if (myScene.objTab[i].type == "triangle")
              {
                if (hitTriangle(viewRay, myScene.objTab[i], t, n))
                { // Si l'objet intersecte le rayon
                  currentObject = i;
                  currentObjectType = "triangle"; // On le prend comme l'objet 'actuel'
                }
              }
            }
            if (currentObject == -1)
              break;

            // On calcule le point de rencontre entre le rayon et l'objet
            point newStart = viewRay.start.pos + t * viewRay.dir;

            // cout << "(" << newStart.x << ", " << newStart.y << ", " << newStart.z << " )" << endl;
            // On calcule la normale à ce point par rapport à l'objet actuel
            if (currentObjectType == "sphere")
            { // 1er cas, la sphère
              n = newStart - myScene.objTab[currentObject].pos;
            } // Pour cela, on fait juste pt_rencontre - centre de l'objet

            // Pour le cube, n est calculé directement via la fonction hitCube

            // On vérifie juste que la normale est non nulle
            if (n * n == 0.0f)
              break;

            // On normalise la normale (aha)
            n = 1.0f / sqrtf(n * n) * n;

            // On va calculer l'éclairement en fonction également du matériau
            material currentMat = myScene.matTab[myScene.objTab[currentObject].material];

            for (unsigned int j = 0; j < myScene.lgtTab.size(); ++j)
            { // Pour chaque lumière
              light current = myScene.lgtTab[j];
              vecteur dist = current.pos - newStart; // On prend le vecteur entre la source et le point de rencontre
              float t = sqrtf(dist * dist);
              if (t <= 0.0f) // On vérifie que le vecteur dist est non nul (et aussi inf pour les erreurs d'arrondi)
                continue;
              if (n * dist <= 0.0f)
              { // On vérifie que l'on prend uniquement les rayons sortants de l'objet
                continue;
              }

              // On crée le rayon de lumière de reflection, qui part du point de rencontre et va vers la source lumineuse
              ray lightRay;
              lightRay.start.pos = newStart;
              lightRay.dir = (1 / t) * dist;

              float fLightProjection = lightRay.dir * n;

              float lightDist = lightRay.dir * lightRay.dir;
              {
                float temp = lightDist;
			          if ( temp == 0.0f )
				          continue;
                temp = 1.0 / sqrtf(temp);
			          lightRay.dir = temp * lightRay.dir;
                fLightProjection = temp * fLightProjection;
              }
              // Ce vecteur est crée pour la fonction hitCube, qui modifie n en faisant les tests de collision.
              //  Evidemment, on veut faire les calculs avec le n initial
              vecteur n_temp = n;

              // calcul des objets dans l'ombre de cette reflexion
              bool inShadow = false;
              for (unsigned int i = 0; i < myScene.objTab.size(); ++i)
              { // Pour tous les objets

                if ((myScene.objTab[i].type == "sphere") && (hitSphere(lightRay, myScene.objTab[i], t)))
                {                  // Si un objet est touché par le rayon réflechi
                  inShadow = true; // On note inShadow comme étant True (cf. suite)
                  break;
                }

                if ((myScene.objTab[i].type == "cube") && (hitCube(lightRay, myScene.objTab[i], t, n)))
                { // Si un objet est touché par le rayon réflechi
                  inShadow = true;
                  break;
                }
                
                if ((myScene.objTab[i].type == "triangle") && (hitTriangle(lightRay, myScene.objTab[i], t, n)))
                { // Si un objet est touché par le rayon réflechi
                  inShadow = true;
                  break;
                }
              }

              // Si un objet est touché par le rayon réflechi, ca veut dire que la surface actuelle est dans l'ombre ! Donc rien à faire
              // En revanche, si il n'y a aucun objet entre la surface actuelle et la lumière :
              if (!inShadow)
              {
                float lambert = (lightRay.dir * n_temp) * coef;
                output.red += lambert * current.couleur.red * currentMat.diffuse.red;
                output.green += lambert * current.couleur.green * currentMat.diffuse.green;
                output.blue += lambert * current.couleur.blue * currentMat.diffuse.blue;

				        // Blinn 
                // La direction de Blinn est exactement à mi chemin entre le rayon
                // lumineux et le rayon de vue. 
                // On calcule le vecteur de Blinn et on le rend unitaire
                // puis on calcule le coefficient de blinn
                // qui est la contribution spéculaire de la lumière courante.

                float fViewProjection = viewRay.dir * n_temp;
                vecteur blinnDir = lightRay.dir - viewRay.dir;
                float temp = blinnDir * blinnDir;
                if (temp != 0.0f )
                {
                  float blinn = (1.0 /sqrtf(temp)) * max(fLightProjection - fViewProjection , 0.0f);
                  blinn = coef * powf(blinn, currentMat.power);
                  output += blinn *currentMat.specular  * current.couleur;
                }
			        }
            }
		        coef *= currentMat.reflection;
		        float reflet = 2.0f * (viewRay.dir * n);
            viewRay.start.pos = newStart;
		        viewRay.dir = viewRay.dir - reflet * n;
            level++;
          } while ((coef > 0.0f) && (level < 10));

          // modification de l'exposition : On définit le terme exposure comme on l'entend entre 0 et -1
          float exposure = -0.3f;
          output.blue = 1.0f - expf(output.blue * exposure);
          output.red = 1.0f - expf(output.red * exposure);
          output.green = 1.0f - expf(output.green * exposure);

          output.blue += sampleRatio * output.blue;
          output.red += sampleRatio * output.red;
          output.green += sampleRatio * output.green; 
        }
      }

      // Transformation gamma pour augmenter la qualité
      float invgamma = 0.45454545; // invgamma est égale à la valeur retenue par le standard sRGB
      output.blue = powf(output.blue, invgamma);
      output.red = powf(output.red, invgamma);
      output.green = powf(output.green, invgamma);

      // On met à jour le pixel de l'image
      imageFile.put((unsigned char)min(output.blue * 255.0f, 255.0f)).put((unsigned char)min(output.green * 255.0f, 255.0f)).put((unsigned char)min(output.red * 255.0f, 255.0f));
    }
  }
  cout << "]\n";

  char outputNamePNG[256];
  sprintf(outputNamePNG, "%.*s.png", (int)(strlen(outputName)-4), outputName);
  int width, height, channels;
  uint8_t* tga_pixels = stbi_load(outputName, &width, &height, &channels, STBI_rgb_alpha);

  // Vérifier si le chargement a réussi
  if (!tga_pixels) 
  {
    std::cout << "Erreur lors du chargement du fichier TGA" << std::endl;
    return -1;
  }

  // Écrire le fichier PNG en utilisant la bibliothèque stb_image_write
  if (!stbi_write_png(outputNamePNG, width, height, STBI_rgb_alpha, tga_pixels, width * 4)) 
  {
    std::cout << "Erreur lors de l'écriture du fichier PNG" << std::endl;
    return -1;
  }
  
  // Libérer la mémoire allouée pour les pixels TGA
  stbi_image_free(tga_pixels);

  return true;
}