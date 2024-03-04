#include "fem.h"

# ifndef NOPOISSONCREATE

femPoissonProblem *femPoissonCreate(const char *filename)
{
    femGeo* theGeometry = geoMeshCreate(filename);
    femPoissonProblem *theProblem = malloc(sizeof(femPoissonProblem));
    femMesh *theMesh = theGeometry->theElements;
    theProblem->geo  = theGeometry;
    if (theMesh->nLocalNode == 4) {
        theProblem->space = femDiscreteCreate(4,FEM_QUAD);
        theProblem->rule = femIntegrationCreate(4,FEM_QUAD); }
    else if (theMesh->nLocalNode == 3) {
        theProblem->space = femDiscreteCreate(3,FEM_TRIANGLE);
        theProblem->rule = femIntegrationCreate(3,FEM_TRIANGLE); }
    theProblem->system = femFullSystemCreate(theMesh->nodes->nNodes);
    return theProblem;
}

# endif
# ifndef NOPOISSONBOUNDARY

void femPoissonFindBoundaryNodes(femPoissonProblem *theProblem)
{
    femGeo* theGeometry = theProblem->geo;  
    femMesh* theEdges = theGeometry->theEdges; 
    int nBoundary = 0;
    femDomain *theBoundary = malloc(sizeof(femDomain));
    nBoundary = theEdges->nElem;
    theGeometry->nDomains++;


    theGeometry->theDomains = realloc(theGeometry->theDomains,theGeometry->nDomains*sizeof(femDomain*));

    theGeometry->theDomains[theGeometry->nDomains-1] = theBoundary;
    theBoundary->nElem = nBoundary;
    theBoundary->elem = malloc(nBoundary*sizeof(int));
    theBoundary->mesh = NULL;
    sprintf(theBoundary->name,"Boundary");

    for (int i = 0; i < nBoundary; i++) {

        theBoundary->elem[i] = theEdges->elem[2*i];
    }
}
    
# endif
# ifndef NOPOISSONFREE

void femPoissonFree(femPoissonProblem *theProblem)
{

    femFullSystemFree(theProblem->system);
    femIntegrationFree(theProblem->rule);
    femDiscreteFree(theProblem->space);
    geoMeshFree(theProblem->geo);
    free(theProblem);
}
    
# endif
# ifndef NOPOISSONLOCAL

void femPoissonLocal(femPoissonProblem *theProblem, const int iElem, int *map, double *x, double *y)
{
    femMesh *theMesh = theProblem->geo->theElements;
    
    for (int j = 0; j<theMesh->nLocalNode ; j++){

        map[j] = theMesh->elem[theMesh->nLocalNode*iElem + j];
        x[j] = theMesh->nodes->X[map[j]];
        y[j] = theMesh->nodes->Y[map[j]];
    }
}

# endif
# ifndef NOPOISSONSOLVE

void femPoissonSolve(femPoissonProblem *theProblem)
{

    femMesh *theMesh = theProblem->geo->theElements;
    femDomain *theBoundary = geoGetDomain(theProblem->geo,"Boundary");
    femFullSystem *theSystem = theProblem->system;
    femIntegration *theRule = theProblem->rule;
    femDiscrete *theSpace = theProblem->space;
 
    if (theSpace->n > 4) Error("Unexpected discrete space size !");  
    double x[4],y[4],phi[4],dphidxsi[4],dphideta[4],dphidx[4],dphidy[4];
    int iElem,iInteg,l,i,j,map[4];

    int nLocal = theMesh->nLocalNode;

    for (iElem = 0; iElem < theMesh->nElem; iElem++){

        femPoissonLocal(theProblem,iElem,map,x,y);
        for (iInteg=0; iInteg < theRule->n; iInteg++){  
            double xsi    = theRule->xsi[iInteg];
            double eta    = theRule->eta[iInteg];
            double weight = theRule->weight[iInteg];  
            femDiscretePhi2(theSpace,xsi,eta,phi);
            femDiscreteDphi2(theSpace,xsi,eta,dphidxsi,dphideta);

            double dxdxsi = 0;
            double dxdeta = 0;
            double dydxsi = 0; 
            double dydeta = 0;
            
            for (i = 0; i < theSpace->n; i++){  
                dxdxsi += x[i]*dphidxsi[i];       
                dxdeta += x[i]*dphideta[i];   
                dydxsi += y[i]*dphidxsi[i];   
                dydeta += y[i]*dphideta[i]; 
            }


            double jac = dxdxsi * dydeta - dxdeta * dydxsi;

            for (i = 0; i < theSpace->n; i++){    
                dphidx[i] = (dphidxsi[i] * dydeta - dphideta[i] * dydxsi) / jac;       
                dphidy[i] = (dphideta[i] * dxdxsi - dphidxsi[i] * dxdeta) / jac; 
            }  

            for (i = 0; i < theSpace->n; i++){ 
                for(j = 0; j < theSpace->n; j++){
                    theSystem->A[map[i]][map[j]] += (dphidx[i] * dphidx[j] + dphidy[i] * dphidy[j]) * jac * weight; 
                }
            }                                                                                            
            for (i = 0; i < theSpace->n; i++){
                theSystem->B[map[i]] += phi[i] * jac *weight; 
            }
        }
    }
    for (l=0; l <theBoundary->nElem; l++){      
       femFullSystemConstrain(theSystem,theBoundary->elem[l],0.0);  
    }
      
    femFullSystemEliminate(theSystem);
}

# endif



