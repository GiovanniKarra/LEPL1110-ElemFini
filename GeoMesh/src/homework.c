#include "fem.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <math.h>

double hermiteInterpolation(double d, double h0, double hStar, double dStar) {
    // Interpolation polynomiale de degré 3
    double f0 = h0;
    double f1 = 0;  // dérivée nulle à l'extrémité
    double f2 = hStar;
    double f3 = 0;  // dérivée nulle à l'extrémité

    // Calcul des coefficients
    double c0 = f0;
    double c1 = f1;
    double c2 = (3 * (f2 - f0) / pow(dStar, 2)) - ((f1 + 2 * f0) / dStar);
    double c3 = (2 * (f0 - f2) / pow(dStar, 3)) + ((f1 + f2) / pow(dStar, 2));

    // Calcul de la valeur interpolée
    double result = c0 + c1 * d + c2 * pow(d, 2) + c3 * pow(d, 3);
    return result;
}

double geoSize(double x, double y){

    femGeo* theGeometry = geoGetGeometry();
    
    double h = theGeometry->h;
    double x0 = theGeometry->xNotch;
    double y0 = theGeometry->yNotch;
    double r0 = theGeometry->rNotch;
    double h0 = theGeometry->hNotch;
    double d0 = theGeometry->dNotch;
  
    
    double x1 = theGeometry->xHole;
    double y1 = theGeometry->yHole;
    double r1 = theGeometry->rHole;
    double h1 = theGeometry->hHole;
    double d1 = theGeometry->dHole;

     
    // Calcul de la distance aux deux cercles
    double distanceToNotch = sqrt(pow(x - x0, 2) + pow(y - y0, 2)) - r0;
    double distanceToHole = sqrt(pow(x - x1, 2) + pow(y - y1, 2)) - r1;

    // Calcul de la taille requise en chaque point du domaine
    double size = h;  // Taille de référence globale
    if (distanceToNotch >= 0 && distanceToNotch <= d0) {
        double sizeNotch = hermiteInterpolation(distanceToNotch, h0, h, d0);
        size = fmin(size, sizeNotch);
    }
    if (distanceToHole >= 0 && distanceToHole <= d1) {
        double sizeHole = hermiteInterpolation(distanceToHole, h1, h, d1);
        size = fmin(size, sizeHole);
    }

    return size;
}

void geoMeshGenerate() {

    femGeo* theGeometry = geoGetGeometry();

    double w = theGeometry->LxPlate;
    double h = theGeometry->LyPlate;
     
    double x0 = theGeometry->xNotch;
    double y0 = theGeometry->yNotch;
    double r0 = theGeometry->rNotch;
    
    
    double x1 = theGeometry->xHole;
    double y1 = theGeometry->yHole;
    double r1 = theGeometry->rHole;
 
//
//  -1- Construction de la g�om�trie avec OpenCascade
//      On cr�e le rectangle
//      On cr�e les deux cercles
//      On soustrait les cercles du rectangle :-)
//
 
    int ierr;
    int idPlate = gmshModelOccAddRectangle(x0, y0, 0.0, -2*x0, -2*y0, 1, 0.0, &ierr);   
    ErrorGmsh(ierr);

    int idNotch = gmshModelOccAddDisk(x0, y0, 0.0, r0, r0 , 2, NULL, 0, NULL, 0, &ierr);
    ErrorGmsh(ierr);

    int idHole  = gmshModelOccAddDisk(x1, y1, 0.0, r1, r1, 3 ,NULL, 0, NULL, 0, &ierr);    
    ErrorGmsh(ierr);
    
    int plate[] = {2, idPlate};
    int notch[] = {2, idNotch};
    int hole[]  = {2, idHole};
    gmshModelOccCut(plate, sizeof(plate) / sizeof(plate[0]), notch, sizeof(notch) / sizeof(notch[0]), NULL, NULL, NULL, NULL, NULL, -1, 1, 1, &ierr);
    ErrorGmsh(ierr);
    gmshModelOccCut(plate, sizeof(plate) / sizeof(plate[0]), hole, sizeof(hole) / sizeof(hole[0]), NULL, NULL, NULL, NULL, NULL, -1, 1, 1, &ierr);
 
//
//  -2- D�finition de la fonction callback pour la taille de r�f�rence
//      Synchronisation de OpenCascade avec gmsh
//      G�n�ration du maillage (avec l'option Mesh.SaveAll :-)      
   
    geoSetSizeCallback(geoSize);
                                  
    gmshModelOccSynchronize(&ierr);       
    gmshOptionSetNumber("Mesh.SaveAll", 1, &ierr);
    gmshModelMeshGenerate(2, &ierr);  
       
///
//  Generation de quads :-)

//    gmshOptionSetNumber("Mesh.SaveAll", 1, &ierr);
//    gmshOptionSetNumber("Mesh.RecombineAll", 1, &ierr);
//    gmshOptionSetNumber("Mesh.Algorithm", 8, &ierr);  
//    gmshOptionSetNumber("Mesh.RecombinationAlgorithm", 1.0, &ierr);
//    gmshModelGeoMeshSetRecombine(2,1,45,&ierr);  
//    gmshModelMeshGenerate(2, &ierr);  
   
 
//
//  Plot of Fltk
//
   gmshFltkInitialize(&ierr);
   gmshFltkRun(&ierr); 

    
}
