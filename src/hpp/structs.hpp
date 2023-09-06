using namespace std;
#include <iostream>
#include <vector>


/*************Définition des différentes structures que l'on va utiliser *****************/

// Un point dans l'espace 3D dans les coordonées cartésiennes
struct point {
	float x, y, z;
};

// Retourne les coordonnées d'un point
istream & operator >> ( istream &inputFile,  point& p ) {
	return inputFile >> p.x >> p.y >> p.z ; 
}

// Un vecteur, également composé de 3 points
struct vecteur {
	float x, y, z;
};

// Retourne les coordonées d'un vecteur
istream & operator >> ( istream &inputFile,  vecteur& v ) {
	return inputFile >> v.x >> v.y >> v.z ; 
}

// Une couleur : RGB (on aurait aussi pu la définir par un point)
struct color {
	float red, green, blue;
};

// Retourne les valeurs RGB d'une couleur
istream & operator >> ( istream &inputFile,  color& couleur) {
	return inputFile >> couleur.red >> couleur.green >> couleur.blue; 
}
// Définition des opérations 

// Multiplie 2 couleurs
inline color operator * (const color&c1, const color &c2 ) {
	color c = {c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue};
	return c;
}

// Additionne 2 couleurs
inline color operator + (const color&c1, const color &c2 ) {
	color c = {c1.red + c2.red, c1.green + c2.green, c1.blue + c2.blue};
	return c;
}

// Ajout du += par souci de simplicité
inline color & operator += (color&c1, const color &c2 ) {
	c1.red +=  c2.red;
    c1.green += c2.green;
    c1.blue += c2.blue;
	return c1;
}

// Multiplication par un coefficient
inline color operator * (float coef, const color &c ) {
	color c2 = {c.red * coef, c.green * coef, c.blue * coef};
	return c2;
}

// Déplace un point en utilisant un vecteur
point operator + (const point&p, const vecteur &v){
	point p2={p.x + v.x, p.y + v.y, p.z + v.z };
	return p2;
}

// Pareil pour la soustraction (gain de temps)
point operator - (const point&p, const vecteur &v){
	point p2={p.x - v.x, p.y - v.y, p.z - v.z };
	return p2;
}

// Addition d'un float à tous les composants d'un vecteur
point operator + (const point&p, const float &f){
	point p2={p.x +f, p.y +f, p.z +f};
	return p2;
}

// Définition d'un vecteur comme différence de 2 points
vecteur operator - (const point&p1, const point &p2){
	vecteur v={p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	return v;
}

// Multiplication d'un vecteur par un entier
vecteur operator * (float c, const vecteur &v)
{
	vecteur v2={v.x *c, v.y * c, v.z * c };
	return v2;
}

// Addition de 2 vecteurs
vecteur operator + (const vecteur&v1, const vecteur &v2){
	vecteur v={v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return v;
}

// Soustraction de 2 vecteurs
vecteur operator - (const vecteur&v1, const vecteur &v2){
	vecteur v={v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	return v;
}

// Division point par point de 2 vecteurs : dans nos calculs, cela renverra un point 
point operator / (const vecteur&v1, const vecteur &v2) {
	return { (v1.x / v2.x),(v1.y / v2.y), (v1.z / v2.z)};
}

// Produit scalaire
float operator * (const vecteur&v1, const vecteur &v2 ) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Produit vectoriel
vecteur operator & (const vecteur&v1, const vecteur &v2) {
	return { (v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};
}


// Définition d'un matériau par :
//	- Sa couleur diffusée
//	- Son indice de réflection
//  - Sa couleur de réflection spéculaire
//  - Sa puissance de réflection spéculaire

struct material {
	color diffuse;
    float reflection;
    color specular;
    float power;
};

// Retourne les pourcentages de couleur et l'indice de reflection d'un matériau
istream & operator >> ( istream &inputFile, material& mat ) {
	return inputFile >> mat.diffuse >> mat.reflection >> mat.specular >> mat.power;
}

// On définit une source lumineuse par sa position et sa couleur
struct light {
	point pos;
	color couleur;
};

// Retourne les valeurs d'une source lumineuse
istream & operator >> ( istream &inputFile, light& lig ) {
	return inputFile >> lig.pos >> lig.couleur;
}

// Un rayon lumineux a une source et va dans une direction
//Pas réellement besoin d'un opérateur >> dans ce cas.
struct ray {
	light start;
	vecteur dir;
};




