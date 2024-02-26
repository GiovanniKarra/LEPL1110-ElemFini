#include "fem.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <math.h>

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
    int idPlate = gmshModelOccAddRectangle(x0, y0, 0.0, -x0, -y0, 0, 0.0, &ierr);   
    ErrorGmsh(ierr);

    int idNotch = gmshModelOccAddDisk(theGeometry->xNotch, theGeometry->yNotch, 0.0, r0, r0 , 1, NULL, 0, NULL, 0, &ierr);
    ErrorGmsh(ierr);

    int idHole  = gmshModelOccAddDisk(theGeometry->xHole, theGeometry->yHole, 0.0, r1, r1, 2 ,NULL, 0, NULL, 0, &ierr);    
    ErrorGmsh(ierr);
    
    int plate[] = {idPlate, 2};
    int notch[] = {idNotch, 2};
    int hole[]  = {idHole, 2};
    gmshModelOccCut(plate, 1, notch, 1, NULL, NULL, NULL, NULL, NULL, -1, 1, 1, &ierr);
    ErrorGmsh(ierr);
    gmshModelOccCut(plate, 1, hole, 1, NULL, NULL, NULL, NULL, NULL, -1, 1, 1, &ierr);
 
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
