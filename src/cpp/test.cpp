#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "../hpp/objects.hpp"

using namespace std;


// Fonction utilisée au fur et à mesure du projet pour tester nos features 
// Notamment pour tester les rotations avec les cubes

int main(){
vecteur dir = {0.0, 0.0, 1.0};
point pos = {50,50,-30};

ray r = {pos, 0,0,0, dir};

object c = {{50,50,0.0},10.0,0,"triangle", 0, 0, 0};
float t = INFINITY;
vecteur n;

bool intersects = hitTriangle(r, c, t,n);
if (intersects == true)
    cout << "Intersecte";
    cout << endl << t << endl;
    cout << n.x << ", " << n.y << ", " << n.z << endl;

}