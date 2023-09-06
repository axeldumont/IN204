#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "structs.hpp"
#include <list>

using namespace std;
float eps = 1e-3;
/********************************* STRUCTURE GLOBALE D'OBJET *********************************/
// La structure des objets : 
//	- Un centre de gravité pos
//	- Une taille (dont la définition diffère pour chaque objet que l'on va décrire)
//	- Un matériau

struct object {
	point pos;
	float size;
	int material;
	string type;
	float angle_rot_x;
  float angle_rot_y;
  float angle_rot_z;
};

istream & operator >> ( istream &inputFile, object& obj ) {
	return inputFile >> obj.pos >> obj.size >> obj.material >> obj.type >> obj.angle_rot_x >> obj.angle_rot_y >> obj.angle_rot_z;
}

// Notre scène est constitué d'un certain nombre de :
//	- Matériaux
// 	- Objets (sphères, cubes, etc...)
// 	- Sources lumineuses
struct scene {

	vector<material> matTab;
	vector<object>   objTab;
	vector<light>    lgtTab;
	int sizex, sizey;         // La taille de la scène
};
/*******************************************************************************************/

/**********************************  OBJET 1 : SPHERE **************************************/

// Le test d'intersection est simple et mathématique car la sphère est un objet très simple
bool hitSphere(const ray &r, const object &s, float &t) 
 { 
    
   // Calcul du discriminant
   vecteur dist = s.pos - r.start.pos;               // Vecteur view_ray -> centre de la sphère
   float B = r.dir * dist;                           // Produit scalaire entre la dist et la direction du rayon
   float D = B*B - dist * dist + s.size * s.size;    // Discriminant de l'équation polynomiale
   
   // On test si il y a intersection 
   if (D < 0) 
     return false; 
   
   // Si il y a bien intersection, on calcule les 2 racines
   float t0 = B - sqrtf(D); 
   float t1 = B + sqrtf(D);
   
   bool retvalue = false; 

   // Si la première racine est positive ET plus petite que t (cf. la suite), on la met comme nouvelle intersection la plus proche
   if ((t0 > 0.1f) && (t0 < t)) 
   {
     t = t0;
     retvalue = true; 
   } 

   // Pareil pour la 2eme racine
   if ((t1 > 0.1f) && (t1 < t)) 
   {
     t = t1; 
     retvalue = true; 
   }

   //On retourne True uniquement si un pt d'intersection a été trouvé
   return retvalue; 
 }

/*******************************************************************************************/

/***********************************  OBJET 2 : CUBE ***************************************/

/******* ROTATION DANS LE REPERE DU CUBE *******/

// Fonction pour tourner un point dans le repère du cube
point transformPoint(const point &p, const object& c){
  
  float angle_x = c.angle_rot_x * M_PI /180;    // Conversion de l'angle en radians
  float angle_y = c.angle_rot_y * M_PI /180;
  float angle_z = c.angle_rot_z * M_PI /180;

  point q;
  // Matrice de rotation en X
  float matrix_x[3][3] = {
    {1, 0, 0},
    {0, cos(angle_x), -sin(angle_x)},
    {0, sin(angle_x), cos(angle_x)}
  };

  // Matrice de rotation en Y
  float matrix_y[3][3] = {
    {cos(angle_y), 0, sin(angle_y)},
    {0, 1, 0},
    {-sin(angle_y), 0, cos(angle_y)}
  };

  // Matrice de rotation en Z 
  float matrix_z[3][3] = {
    {cos(angle_z), -sin(angle_z), 0},
    {sin(angle_z), cos(angle_z), 0},
    {0, 0, 1 }
  };

  // Soustraire le centre pour la rotation par rapport au centre
  float x = p.x - c.pos.x;
  float y = p.y - c.pos.y; 
  float z = p.z - c.pos.z;

    // Appliquer la rotation en X
    float x1 = matrix_x[0][0] * x + matrix_x[0][1] * y + matrix_x[0][2] * z;
    float y1 = matrix_x[1][0] * x + matrix_x[1][1] * y + matrix_x[1][2] * z;
    float z1 = matrix_x[2][0] * x + matrix_x[2][1] * y + matrix_x[2][2] * z;

    // Appliquer la rotation en Y
    float x2 = matrix_y[0][0] * x1 + matrix_y[0][1] * y1 + matrix_y[0][2] * z1;
    float y2 = matrix_y[1][0] * x1 + matrix_y[1][1] * y1 + matrix_y[1][2] * z1;
    float z2 = matrix_y[2][0] * x1 + matrix_y[2][1] * y1 + matrix_y[2][2] * z1;

    // Appliquer la rotation en Z
    float x3 = matrix_z[0][0] * x2 + matrix_z[0][1] * y2 + matrix_z[0][2] * z2;
    float y3 = matrix_z[1][0] * x2 + matrix_z[1][1] * y2 + matrix_z[1][2] * z2;
    float z3 = matrix_z[2][0] * x2 + matrix_z[2][1] * y2 + matrix_z[2][2] * z2;

    // Ajouter le centre du cube pour recentrer le sommet
    q.x = x3 + c.pos.x;
    q.y = y3 + c.pos.y;
    q.z = z3 + c.pos.z;

    return q;
  }

// Fonction pour tourner un vecteur dans le repère du cube
vecteur transformVect(const vecteur &p, const object& c){
  
  float angle_x = c.angle_rot_x *  0.0174533;    // Conversion de l'angle en radians
  float angle_y = c.angle_rot_y *  0.0174533;
  float angle_z = c.angle_rot_z * M_PI /180;
  
  //cout << angle_x << endl;
  //cout << angle_y <<endl;
  vecteur q;

  // Matrice de rotation en X
  float matrix_x[3][3] = {
    {1, 0, 0},
    {0, cos(angle_x), -sin(angle_x)},
    {0, sin(angle_x), cos(angle_x)}
  };

  // Matrice de rotation en Y
  float matrix_y[3][3] = {
    {cos(angle_y), 0, sin(angle_y)},
    {0, 1, 0},
    {-sin(angle_y), 0, cos(angle_y)}
  };

  // Matrice de rotation en Z 
  float matrix_z[3][3] = {
    {cos(angle_z), -sin(angle_z), 0},
    {sin(angle_z), cos(angle_z), 0},
    {0, 0, 1 }
  };

  float x = p.x;
  float y = p.y;
  float z = p.z;

    // Appliquer la rotation en X
    float x1 = matrix_x[0][0] * x + matrix_x[0][1] * y + matrix_x[0][2] * z;
    float y1 = matrix_x[1][0] * x + matrix_x[1][1] * y + matrix_x[1][2] * z;
    float z1 = matrix_x[2][0] * x + matrix_x[2][1] * y + matrix_x[2][2] * z;

    // Appliquer la rotation en Y
    float x2 = matrix_y[0][0] * x1 + matrix_y[0][1] * y1 + matrix_y[0][2] * z1;
    float y2 = matrix_y[1][0] * x1 + matrix_y[1][1] * y1 + matrix_y[1][2] * z1;
    float z2 = matrix_y[2][0] * x1 + matrix_y[2][1] * y1 + matrix_y[2][2] * z1;

    // Appliquer la rotation en Z
    float x3 = matrix_z[0][0] * x2 + matrix_z[0][1] * y2 + matrix_z[0][2] * z2;
    float y3 = matrix_z[1][0] * x2 + matrix_z[1][1] * y2 + matrix_z[1][2] * z2;
    float z3 = matrix_z[2][0] * x2 + matrix_z[2][1] * y2 + matrix_z[2][2] * z2;
    
    q.x = x3;
    q.y = y3;
    q.z = z3;

    return q;
  }

/***** MAINTENANT, LES FONCTIONS POUR LE RETOUR DANS LE REPERE GLOBAL ******/

point transformPointInverse(const point& p, const object& c) {
    
  float angle_x = c.angle_rot_x *  0.0174533;    // Conversion de l'angle en radians
  float angle_y = c.angle_rot_y *  0.0174533;
  float angle_z = c.angle_rot_z * M_PI /180;

  point q;
  
  // Matrice de rotation en X INVERSEE
  float matrix_x[3][3] = {
    {1, 0, 0},
    {0, cos(angle_x), sin(angle_x)},
    {0, -sin(angle_x), cos(angle_x)}
  };

  // Matrice de rotation en Y INVERSEE
  float matrix_y[3][3] = {
    {cos(angle_y), 0, -sin(angle_y)},
    {0, 1, 0},
    {sin(angle_y), 0, cos(angle_y)}
  };

  // Matrice de rotation en Z INVERSEE
  float matrix_z[3][3] = {
    {cos(angle_z), sin(angle_z), 0},
    {-sin(angle_z), cos(angle_z), 0},
    {0, 0, 1 }
  };

  // Soustraire le centre pour la rotation par rapport au centre
  float x = p.x - c.pos.x;
  float y = p.y - c.pos.y;
  float z = p.z - c.pos.z;

    // Appliquer la rotation en X
    float x1 = matrix_x[0][0] * x + matrix_x[0][1] * y + matrix_x[0][2] * z;
    float y1 = matrix_x[1][0] * x + matrix_x[1][1] * y + matrix_x[1][2] * z;
    float z1 = matrix_x[2][0] * x + matrix_x[2][1] * y + matrix_x[2][2] * z;

    // Appliquer la rotation en Y
    float x2 = matrix_y[0][0] * x1 + matrix_y[0][1] * y1 + matrix_y[0][2] * z1;
    float y2 = matrix_y[1][0] * x1 + matrix_y[1][1] * y1 + matrix_y[1][2] * z1;
    float z2 = matrix_y[2][0] * x1 + matrix_y[2][1] * y1 + matrix_y[2][2] * z1;

    // Appliquer la rotation en Z
    float x3 = matrix_z[0][0] * x2 + matrix_z[0][1] * y2 + matrix_z[0][2] * z2;
    float y3 = matrix_z[1][0] * x2 + matrix_z[1][1] * y2 + matrix_z[1][2] * z2;
    float z3 = matrix_z[2][0] * x2 + matrix_z[2][1] * y2 + matrix_z[2][2] * z2;

    // Ajouter le centre du cube pour recentrer le sommet
    q.x = x3 + c.pos.x;
    q.y = y3 + c.pos.y;
    q.z = z3 + c.pos.z;

    return q;
  }

vecteur transformVectInverse(const vecteur &p, const object& c){
  
  float angle_x = c.angle_rot_x *  0.0174533;    // Conversion de l'angle en radians
  float angle_y = c.angle_rot_y *  0.0174533;
  float angle_z = c.angle_rot_z * M_PI /180;

  vecteur q;

  // Matrice de rotation en X INVERSEE
  float matrix_x[3][3] = {
    {1, 0, 0},
    {0, cos(angle_x), sin(angle_x)},
    {0, -sin(angle_x), cos(angle_x)}
  };

  // Matrice de rotation en Y INVERSEE
  float matrix_y[3][3] = {
    {cos(angle_y), 0, -sin(angle_y)},
    {0, 1, 0},
    {sin(angle_y), 0, cos(angle_y)}
  };

  // Matrice de rotation en Z INVERSEE
  float matrix_z[3][3] = {
    {cos(angle_z), sin(angle_z), 0},
    {-sin(angle_z), cos(angle_z), 0},
    {0, 0, 1 }
  };

  float x = p.x;
  float y = p.y;
  float z = p.z;

    // Appliquer la rotation en X
    float x1 = matrix_x[0][0] * x + matrix_x[0][1] * y + matrix_x[0][2] * z;
    float y1 = matrix_x[1][0] * x + matrix_x[1][1] * y + matrix_x[1][2] * z;
    float z1 = matrix_x[2][0] * x + matrix_x[2][1] * y + matrix_x[2][2] * z;

    // Appliquer la rotation en Y
    float x2 = matrix_y[0][0] * x1 + matrix_y[0][1] * y1 + matrix_y[0][2] * z1;
    float y2 = matrix_y[1][0] * x1 + matrix_y[1][1] * y1 + matrix_y[1][2] * z1;
    float z2 = matrix_y[2][0] * x1 + matrix_y[2][1] * y1 + matrix_y[2][2] * z1;

    // Appliquer la rotation en Z
    float x3 = matrix_z[0][0] * x2 + matrix_z[0][1] * y2 + matrix_z[0][2] * z2;
    float y3 = matrix_z[1][0] * x2 + matrix_z[1][1] * y2 + matrix_z[1][2] * z2;
    float z3 = matrix_z[2][0] * x2 + matrix_z[2][1] * y2 + matrix_z[2][2] * z2;

    q.x = x3;
    q.y = y3;
    q.z = z3;

    return q;
  }

ray transformRayInverse(const ray& r, const object& c) {
    
    point origin = transformPointInverse(r.start.pos, c);
    vecteur direction = transformVectInverse(r.dir, c);

    return {{origin,r.start.couleur}, direction};
}

/*
Fonction qui vérifie si le rayon intersecte bien le cube.
La fonction actualise aussi t : distance rayon-intersection 
                         et n : vecteur normal au point d'intersection.
Comme le cube est tourné selon les axes x et y, voici la méthode utilisée :
  1) Mettre le rayon dans le système de coordonées du cube. 
     Pour cela, on muplitiplie le rayon (origine et direction) par l'inverse de la matrice de rotation du cube.
  2) Calculer l'intersection et la normale de ce rayon avec le cube non tourné (simple quand c'est non tourné).
  3) Remettre ce point d'intersection et cette normale dans le repère global en utilisant la matrice de rotation du cube.
*/

bool hitCube(const ray &r1, const object &c, float &t, vecteur &n) 
 {
  // Notation utile
  point center = c.pos;

  /******************* PARTIE 1 **************************/
  ray r = transformRayInverse(r1,c);

  /*************** Partie 2 : Algo de type AABB (Bounding Box) *************/

  // Calcul des 2 points extrémaux qui définissent le cube dans les axes du repère
  point AABB_min = {center.x - c.size / 2, center.y - c.size / 2, center.z - c.size / 2};
  point AABB_max = {center.x + c.size / 2, center.y + c.size / 2, center.z + c.size / 2};
  
  // Calcul des 2 distances maximales entre l'origine du rayon et les pts du cube
  point tMin = (AABB_min - r.start.pos) / r.dir;
  point tMax= (AABB_max - r.start.pos) / r.dir;
  
  // Ajouté pour fixer un bug sur les cubes quand on a des angles de rotation nuls
  if(c.angle_rot_x == 0 || c.angle_rot_y == 0)
    if (fabs(r.start.pos.y - AABB_min.y) <0.001 || fabs(r.start.pos.x - AABB_min.x) <0.001 || fabs(r.start.pos.x - AABB_max.x) <0.001 || fabs(r.start.pos.y - AABB_max.y) <0.001 || fabs(r.start.pos.z - AABB_max.z) <0.001 || fabs(r.start.pos.z - AABB_max.z) <0.001)
      return false;

  // On inverse si jamais 
  if (tMin.x > tMax.x) {
    swap(tMin.x, tMax.x);
    }
  if (tMin.y > tMax.y) {
    swap(tMin.y, tMax.y);
    }
  if (tMin.z > tMax.z) {
    swap(tMin.z, tMax.z);
    }
  
  // Pas d'intersection : le rayon est extérieur à la boite sur les axes x ou y
  if (tMin.x > tMax.y || tMin.y > tMax.x) {
        return false;
    }
  // Sinon, on prend le minimum des tMin et le max des tMax. On les stocke dans la coordonnée x.
  if (tMin.y > tMin.x) {
        tMin.x = tMin.y;
    }
  if (tMax.y < tMax.x) {
        tMax.x = tMax.y;
    }

  // De même selon z
  if (tMin.x > tMax.z || tMin.z > tMax.x) {
        return false;
    }

  if (tMin.z > tMin.x) {
        tMin.x = tMin.z;
    }
  if (tMax.z < tMax.x) {
        tMax.x = tMax.z;
    }

    if (tMin.x < 0.1f || tMin.x > t) {
        return false;
    }
 
    t = tMin.x;

    point intersectionPoint = r.start.pos + t * r.dir;

    vecteur normal;
    if ( fabs(intersectionPoint.x - AABB_min.x) < 1.0f) {
        normal = {-1, 0, 0};
    }
    else if (fabs(intersectionPoint.x -AABB_max.x) < 1.0f) {
        normal = {1, 0, 0};
    }
    else if (fabs(intersectionPoint.y - AABB_min.y) < 1.0f) {
        normal = {0, -1, 0};
    }
    else if ( fabs(intersectionPoint.y - AABB_max.y) < 1.0f) {
        normal = {0, 1, 0};
    }
    else if (fabs(intersectionPoint.z - AABB_min.z) < 1.0f) {
        normal = {0, 0, -1};
    }
    else if ( fabs(intersectionPoint.z - AABB_max.z) < 1.0f) {
        normal = {0, 0, 1};
    }

    /************PARTIE 3 : ON REMET LA NORMALE DANS LE REPERE GLOBAL **************/
  
    // On remet la normale dans les coordonnées de la fenêtre
    n = transformVect(normal,c);

    return true;

 }

/*******************************************************************************************/

/***********************************  OBJET 3 : Le triangle ********************************/

// Element principal de rendu dans les jeux, etc...
// On utilise l'algo de Möller-Trumbore remanié à notre sauce
// On va faire comme pour le cube : changement de repère, etc...
// De base, on a un triangle équilatéral "droit" avec un sommet en haut du milieu de la base
// Ici, c.size sera la distance centre-sommet

bool hitTriangle(const ray &r1, const object &c, float &t, vecteur &n) 
{
// Notation utile
point center = c.pos;
/******************* PARTIE 1 **************************/
ray r = transformRayInverse(r1,c);

/*************** Partie 2 : Algo de Möller-Trumbore *************/
  
// Calcul des 3 sommets ds le repère du triangle
point s1 = {center.x, center.y + 2*c.size, center.z};
point s2 = {center.x + c.size * sinf(2 * M_PI / 3), center.y + c.size * cosf(2 * M_PI / 3), center.z};
point s3 = {center.x + c.size * sinf(4 * M_PI / 3), center.y + c.size * cosf(4 * M_PI / 3), center.z};

// Vecteurs des arêtes
vecteur v1 = s2-s1;
vecteur v2 = s3-s1;

//Calcul du produit vectoriel rayon - coté du triangle
vecteur pvec = r.dir & v2;
float det =  v1 * pvec;

// Cas où le rayon est parallèle au triangle
  if (fabs(det) ==0)
    return false;

// Sinon, on poursuit les calculs
float inv_det = 1.0f / det;
vecteur tvec = r.start.pos - s1;

//u et v seront les coordonées barycentriques du point d'intersection
float u = (tvec * pvec) * inv_det;

// Le point d'intersection est en dehors du triangle
if (u < eps || u > 1.0f +eps)
    return false;

vecteur qvec = tvec & v1;
float v = (r.dir *qvec) * inv_det;

// Le point d'intersection est en dehors du triangle
if (v < -eps || u + v > 1.0f +eps)
    return false;

float temp = (v2 * qvec)* inv_det;
if (temp >t  || temp < 0.5)
  return false;

t = temp;
//Calcul de la normale
vecteur normal = v1 & v2;

if (c.angle_rot_x >90)
 normal = v2 & v1;

// On remet la normale dans les coordonnées de la fenêtre
n = transformVect(normal,c);


return true;
}










































/************************ RELIQUES DE CODE *****************************/

/*
bool hitCube(const ray &r1, const object &c, float &t, vecteur &n) 
 {  
    point center = c.pos;           // Le centre du cube
    int size = c.size;              // La taille d'un côté du cube
    float angle_x = c.angle_rot_x; // L'angle de rotation du cube
    float angle_y = c.angle_rot_y;
    angle_x = angle_x *  0.0174533;    // Conversion de l'angle en radians
    angle_y = angle_y *  0.0174533;
    
    // les sommets du cube
    //  - Face gauche (1) : 4 premiers sommets
    //  - Face droite (2) : 4 derniers sommets
    //  - Face avant  (3) : Sommets d'indice pairs
    //  - Face arrière(4) : Sommets d'indice impairs
    //  - Face dessus (5) : Sommets 2,3,6,7
    //  - Face dessous(6) : Sommets 0,1,4,5

    point vertices[8] = {
      {center.x - size / 2, center.y - size / 2, center.z - size / 2}, // Sommet en bas à gauche devant
      {center.x - size / 2, center.y - size / 2, center.z + size / 2}, // Sommet en bas à gauche derriere
      {center.x - size / 2, center.y + size / 2, center.z - size / 2}, // Sommet en haut à gauche devant
      {center.x - size / 2, center.y + size / 2, center.z + size / 2}, // Sommet en haut à gauche derrière
      {center.x + size / 2, center.y - size / 2, center.z - size / 2}, // Sommet en bas à droite devant
      {center.x + size / 2, center.y - size / 2, center.z + size / 2}, // Sommet en bas à droite derrière
      {center.x + size / 2, center.y + size / 2, center.z - size / 2}, // Sommet en haut à droite devant
      {center.x + size / 2, center.y + size / 2, center.z + size / 2}  // Sommet en haut à droite derrière
    }; 

  
  ray r = transformRayInverse(r1,c);
  
  // Rotation du cube 
  rotateCube(vertices, c);

  // Faces du cube 
  face cube[6] = {
      {vertices[0], vertices[2], vertices[4],vertices[6]}, // Face avant intialement
      {vertices[7], vertices[7], vertices[5],vertices[1]}, // Face arrière initialement
      {vertices[0], vertices[1], vertices[2],vertices[3]}, // Face gauche
      {vertices[7], vertices[7], vertices[4],vertices[5]}, // Face droite
      {vertices[2], vertices[3], vertices[6],vertices[7]}, // Face dessus
      {vertices[0], vertices[0], vertices[1],vertices[5]}, // Face dessous
    };

  // calcule les vecteurs normaux des 6 faces du cube
  vecteur normales[6];
  for (int i=0; i<6;i++){
    vecteur diago1 = cube[i].p1 - cube[i].p4;
    //cout << diago1.x << ", " <<diago1.y << ", " <<diago1.z << endl;
    vecteur diago2 = cube[i].p2 - cube[i].p3;
    //cout << diago2.x << ", " <<diago2.y << ", " <<diago2.z << endl;
    vecteur k = diago1 & diago2;
    float norm = fabs(1/sqrtf((k.x)*(k.x) + (k.y)*(k.y) + (k.z)*(k.z)));
    normales[i] = norm * k;

    if(fabs(normales[i].x)<0.0001)
      normales[i].x =0;
    if(fabs(normales[i].y)<0.0001)
      normales[i].y =0;
    if(fabs(normales[i].z)<0.0001)
      normales[i].z =0;
    //cout << normales[i].x << ", " << normales[i].y << ", " << normales[i].z << endl;
    // cout << endl;
  }

  // Tests des intersections entre le rayon et le cube
    
  // Calculate the minimum and maximum x, y, and z values of the cube
  float min_x = (center.x - c.size / 2.0 );
  float max_x = (center.x + c.size / 2.0);
  float min_y = (center.y - c.size / 2.0 );
  float max_y = (center.y + c.size / 2.0);
  float min_z = (center.z - c.size / 2.0 );
  float max_z = (center.z + c.size / 2.0);

  float k; 
  float liste_k[6] = {INFINITY, INFINITY, INFINITY, INFINITY, INFINITY, INFINITY}; // Valeurs quasi à l'infini
  point intersection;
  bool intersects = false;   // True si le rayon intersecte le cube
   
    //Face gauche
    k = (min_x - r.start.pos.x) / r.start.pos.x; 
    intersection.x = min_x;
    intersection.y = r.start.pos.y + k * r.dir.y;
    intersection.z = r.start.pos.z + k * r.dir.z;
    if (intersection.y >= min_y && intersection.y <= max_y &&
        intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
        liste_k[2] = k;
        intersects = true;
    }
    //face droite
    k = (max_x - r.start.pos.x) / r.start.pos.x;
    intersection.x = max_x;
    intersection.y = r.start.pos.y + k * r.dir.y;
    intersection.z = r.start.pos.z + k * r.dir.z;
    if (intersection.y >= min_y && intersection.y <= max_y &&
        intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
        liste_k[3] = k;
        intersects = true;
    }

    //face bas
    k = (min_y - r.start.pos.y) / r.dir.y;
    intersection.x = r.start.pos.x + k * r.dir.x;
    intersection.y = min_y;
    intersection.z = r.start.pos.z + k * r.dir.z;
    if (intersection.x >= min_x && intersection.x <= max_x &&
        intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
        liste_k[4] = k;
        intersects = true;
    }
    //face haut
    k = (max_y - r.start.pos.y) / r.dir.y;
    intersection.x = r.start.pos.x + t * r.dir.x;
    intersection.y = max_y;
    intersection.z = r.start.pos.z + t * r.dir.z;
    if (intersection.x >= min_x && intersection.x <= max_x &&
        intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
        liste_k[5] = k;
        intersects = true;
    }
    //face avant
    k = (min_z - r.start.pos.z) / r.dir.z;
    intersection.x = r.start.pos.x + k * r.dir.x;
    intersection.y = r.start.pos.y + k * r.dir.y;
    intersection.z = min_z;
    if (intersection.x >= min_x && intersection.x <= max_x &&
        intersection.y >= min_y && intersection.y <= max_y && k > 0.0f) {
        liste_k[0] = k;
        intersects = true;
    }
    
    //face arrière
    k = (max_z - r.start.pos.z) / r.dir.z;
    intersection.x = r.start.pos.x + k * r.dir.x;
    intersection.y = r.start.pos.y + k * r.dir.y;
    intersection.z = max_z;
    if (intersection.x >= min_x && intersection.x <= max_x &&
      intersection.y >= min_y && intersection.y <= max_y && k>0.0f) {
      liste_k[1] = k;
      intersects = true;
    }
      
    if (min_element(begin(liste_k),end(liste_k))[0] < t){
      t = min_element(begin(liste_k),end(liste_k))[0];  // Valeur minimale : intersection la plus proche
      auto itr = find(begin(liste_k), end(liste_k), t);
      int num_face = distance(begin(liste_k), itr);
      n = normales[num_face];
      //cout << "face de collision " << num_face << endl;
      intersection = {r.start.pos.x + t * r.dir.x, r.start.pos.y + t * r.dir.y, r.start.pos.z + t * r.dir.z};
      point intersection_reelle = transformPoint(intersection, c);
      t = sqrt((r.start.pos - intersection_reelle) * (r.start.pos - intersection_reelle));}

    //std::cout << "t =" << t << endl;
    return intersects;
 }*/

/*
  point minBounds = vertices[0];
  point maxBounds = vertices[6];
  point intersectionPoint;
  float txmin = (minBounds.x - r.start.pos.x) / r.dir.x;
    float txmax = (maxBounds.x - r.start.pos.x) / r.dir.x;
    float tymin = (minBounds.y - r.start.pos.y) / r.dir.y;
    float tymax = (maxBounds.y - r.start.pos.y) / r.dir.y;
    float tzmin = (minBounds.z - r.start.pos.z) / r.dir.z;
    float tzmax = (maxBounds.z - r.start.pos.z) / r.dir.z;

    // On détermine les valeurs min et max pour t
    auto tmin = std::max(std::max(std::min(txmin, txmax), std::min(tymin, tymax)), std::min(tzmin, tzmax));
    auto tmax = std::min(std::min(std::max(txmin, txmax), std::max(tymin, tymax)), std::max(tzmin, tzmax));

    // On vérifie si l'intersection est valide
    if (tmax >= tmin && tmax >= 0) {
        vecteur normal;
        // Calcul du point d'intersection
        intersectionPoint = r.start.pos + tmin * r.dir;

        // Calcul de la normale au point d'intersection
        if (std::abs(intersectionPoint.x - minBounds.x) < 0.001) {
            normal = {-1, 0, 0};
        }
        else if (std::abs(intersectionPoint.x - maxBounds.x) < 0.001) {
            normal = {1, 0, 0};
        }
        else if (std::abs(intersectionPoint.y - minBounds.y) < 0.001) {
            normal = {0, -1, 0};
        }
        else if (std::abs(intersectionPoint.y - maxBounds.y) < 0.001) {
            normal = {0, 1, 0};
        }
        else if (std::abs(intersectionPoint.z - minBounds.z) < 0.001) {
            normal = {0, 0, -1};
        }
        else if (std::abs(intersectionPoint.z - maxBounds.z) < 0.001) {
            normal = {0, 0, 1};
        }

        // on remet le point d'intersection dans les coordonées de la fenêtre
        auto intersectionPointReel = transformPoint(intersectionPoint,c);

        // On remet la normale dans les coordonées de la fenêtre
        n = transformVect(normal,c);

        // On recalcule la vraie distance entre le départ du rayon et le pt d'intersection
        t = sqrt((r1.start.pos - intersectionPointReel) * (r1.start.pos - intersectionPointReel));
        return true;
    }

    return false;
}
*/
  
/*
  double tMin = -INFINITY;
  double tMax = INFINITY;
  double epsilon = 0.00001;
  point intersection;

  float center_coord[3] = {center.z, center.x, center.y};
  float rayOrigin[3] = {r.start.pos.z, r.start.pos.x, r.start.pos.y};
  float rayDirection[3] = {r.dir.z, r.dir.x, r.dir.y};
  int num_face;
  
  float liste_t_min[3]; // Valeurs quasi à l'infini
  float liste_t_max[3];

    for (int i = 0; i < 3; ++i) {
        double t0 = (center_coord[i] - c.size / 2.0 - rayOrigin[i]) / rayDirection[i];
        double t1 = (center_coord[i] + c.size / 2.0 - rayOrigin[i]) / rayDirection[i];
        
        if (t0 > t1) {
            swap(t0, t1);
        }
        if (t0 > tMin) {
            tMin = t0;
            liste_t_min[i] = i;
        }
        if (t1 < tMax) {
            tMax = t1;
            liste_t_max[i] = i;
        } 
        if (tMin > tMax + epsilon) {
            return false;
        }
        if (tMax < 0) {
            return false;
        }
    }
    if (tMin >0){
      t = tMin;
      num_face = 2*min_element(begin(liste_t_min),end(liste_t_min))[0] ;
    } else {
      t = tMax;
      num_face = 2*max_element(begin(liste_t_min),end(liste_t_min))[0]+1;
    }

    intersection = {r.start.pos.x + t * r.dir.x, r.start.pos.y + t * r.dir.y, r.start.pos.z + t * r.dir.z};
    point intersection_reelle = transformPointInverse(intersection, c);

    r = transformRayInverse(r,c);
    r = transformRayInverse(r,c);
    t = sqrt((r.start.pos - intersection_reelle) * (r.start.pos - intersection_reelle));
    n = normales[num_face];

    cout << "n = " << n.x << ", "<< n.y << ", " << n.z << endl;
    return true;
 
}
*/