#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include <cstring>
#include "objects.hpp"

/****************** Fonction d'initialisation du fichier TGA de sortie *********************/

bool header_tga(char* outputName, scene &myScene, ofstream &imageFile){
    
   // Vérification au cas où
   if (!imageFile){
    return false;
   }
   
   //Le format TGA est spécial et doit etre initialisé avec des octets précis.
   // Binaire : On ajoute le header spécifique au format TGA
   imageFile.put(0).put(0);  // - 2 premiers octets à 0
   imageFile.put(2);         // - 3ème octet mis à 2 pour dire que l'on est en RGB non compressé
   imageFile.put(0).put(0);  // - 2 Octets de l'origine en X mis à 0
   imageFile.put(0).put(0);  // - 2 Octets de l'origine en Y mis à 0

   imageFile.put(0);
   imageFile.put(0).put(0); 
   imageFile.put(0).put(0); 

   // Ici, le 0xFF00 sert a prendre uniquement les valeurs octet par octet (c'est un masque)
   // On divise par 256 pour le 2ème octet car diviser par 2^8 décale justement de 8 bits le résultat en binaire
   // Bref, ca sert juste a stocker la taille de l'image
   imageFile.put((myScene.sizex & 0x00FF)).put((myScene.sizex & 0xFF00) / 256); // - 2 octets de la largeur de l'image 
   imageFile.put((myScene.sizey & 0x00FF)).put((myScene.sizey & 0xFF00) / 256); // - 2 octets de la largeur de l'image
   
   imageFile.put(24); // On a 24 bits par pixel dans l'image
   imageFile.put(0);  // On finit le header avec un 0
   
   return true;
}
/*******************************************************************************************/

