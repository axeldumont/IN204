#include "raytracer.cpp"

/************* Initialisation : création de la scène à partir du fichier texte *************/
 bool init(char* inputName, scene &myScene) 
 {
   int nbMat, nbObjets, nbLight;
   int i;

   ifstream sceneFile(inputName);
   if (!sceneFile)
     return  false;
   sceneFile >> myScene.sizex >> myScene.sizey; // 1ère ligne du .txt
   sceneFile >> nbMat >> nbObjets >> nbLight;   // 2ème ligne du .txt
   
   // On dimensionne les vecteurs de notre scène en fonction des valeurs du .txt
   myScene.matTab.resize(nbMat); 
   myScene.objTab.resize(nbObjets); 
   myScene.lgtTab.resize(nbLight); 

   // Maintenant que c'est dimensionné, on remplit ces vecteurs avec les valeurs 
   for (i=0; i < nbMat; i++)             // Informations sur les matériaux (chaque matTab[i] contient 4 valeurs)
     sceneFile >> myScene.matTab[i];
   for (i=0; i < nbObjets; i++){       // Informations sur les objets (chaque objTab[i] contient 8 valeurs)
     sceneFile >> myScene.objTab[i];
    }

   for (i=0; i < nbLight; i++)           // Informations sur les lumières (chaque lgtTab[i] contient 6 valeurs)
     sceneFile >> myScene.lgtTab[i];
   
   return true;
 } 
/*******************************************************************************************/

/**************************************** Fonction main ************************************/
 int main(int argc, char* argv[]) {
   if  (argc < 3){
     cout << "Usage : ./main scene.txt output.tga" << endl;
     return -1;}

   scene myScene;
   
   if (!init(argv[1], myScene)){   // On initialise (indirectement)
     cout << "Echèc de l'ouverture de la scène." << endl;
     return -1;}

   if (!draw(argv[2], myScene)){    // On dessine (toujours indirectement)
     cout << "Echèc de la création de l'image." << endl;
     return -1;}
   
   return 0;
 }

/*******************************************************************************************/