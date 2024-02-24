#include "fem.h"

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




    //
    //     A modifier !
    //     
    // Your contribution starts here ....
    //
    double Lx = 1.0;
    double Ly = 2.0;
    theGeometry->LxPlate = Lx;
    theGeometry->LyPlate = Ly;
    theGeometry->xPlate = 0.0;
    theGeometry->yPlate = 0.0;
    theGeometry->xHole = Lx/4.0;
    theGeometry->yHole = Ly/4.0;
    theGeometry->rHole = Lx/8.0;
    theGeometry->xNotch = -Lx/2.0;
    theGeometry->yNotch = -Ly/2.0;
    theGeometry->rNotch = Lx/2.0;
    
     
    return h;
    
//   
// Your contribution ends here :-)
//

}


#define ___ 0

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
    int idPlate = gmshModelOccAddRectangle(___, ___, ___, ___, ___, ___, ___,&ierr);   
    ErrorGmsh(ierr);
    int idNotch = gmshModelOccAddDisk(___, ___, ___, ___, ___, ___,NULL,0,NULL,0,&ierr); 
    ErrorGmsh(ierr);
    int idHole  = gmshModelOccAddDisk(___, ___, ___, ___, ___, ___,NULL,0,NULL,0,&ierr);    
    ErrorGmsh(ierr);
    
    int plate[] = {___,___};
    int notch[] = {___,___};
    int hole[]  = {___,___};
    gmshModelOccCut(___,___,___,___,NULL,NULL,NULL,NULL,NULL,-1,1,1,&ierr); 
    ErrorGmsh(ierr);
    gmshModelOccCut(___,___,___,___,NULL,NULL,NULL,NULL,NULL,-1,1,1,&ierr); 
    ErrorGmsh(ierr);
 
//
//  -2- D�finition de la fonction callback pour la taille de r�f�rence
//      Synchronisation de OpenCascade avec gmsh
//      G�n�ration du maillage (avec l'option Mesh.SaveAll :-)
                  
   
    geoSetSizeCallback(geoSize);
                                  
    gmshModelOccSynchronize(&ierr);       
    gmshOptionSetNumber("Mesh.SaveAll", 1, &ierr);
    gmshModelMeshGenerate(2, &ierr);  
       
//
//  Generation de quads :-)
//
//    gmshOptionSetNumber("Mesh.SaveAll", 1, &ierr);
//    gmshOptionSetNumber("Mesh.RecombineAll", 1, &ierr);
//    gmshOptionSetNumber("Mesh.Algorithm", 8, &ierr);  chk(ierr);
//    gmshOptionSetNumber("Mesh.RecombinationAlgorithm", 1.0, &ierr);  chk(ierr);
//    gmshModelGeoMeshSetRecombine(2,1,45,&ierr);  chk(ierr);
//    gmshModelMeshGenerate(2, &ierr);  
   
 
//
//  Plot of Fltk
//
//   gmshFltkInitialize(&ierr);
//   gmshFltkRun(&ierr);  chk(ierr);
//
    
}