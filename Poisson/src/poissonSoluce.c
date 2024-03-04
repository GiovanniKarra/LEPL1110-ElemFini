#include "fem.h"

# ifndef NOPOISSONCREATE

femPoissonProblem *femPoissonCreate(const char *filename)
{
    femPoissonProblem *theProblem = malloc(sizeof(femPoissonProblem));
    theProblem->mesh  = femMeshRead(filename);   
    femMeshClean(theProblem->mesh);        
    theProblem->edges = femEdgesCreate(theProblem->mesh);  
    if (theProblem->mesh->nLocalNode == 4) {
        theProblem->space = femDiscreteCreate(4,FEM_QUAD);
        theProblem->rule = femIntegrationCreate(4,FEM_QUAD); }
    else if (theProblem->mesh->nLocalNode == 3) {
        theProblem->space = femDiscreteCreate(3,FEM_TRIANGLE);
        theProblem->rule = femIntegrationCreate(3,FEM_TRIANGLE); }
    theProblem->system = femFullSystemCreate(theProblem->mesh->nNode);
    return theProblem;
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
# ifndef NOMESHLOCAL

void femPoissonLocal(femPoissonProblem *theProblem, const int iElem, int *map, double *x, double *y){
    femMesh *theMesh = theProblem->geo->theElements;
    
    for (int j = 0; j<theMesh->nLocalNode ; j++){
        map[j] = theMesh->elem[theMesh->nLocalNode*iElem + j];
        x[j] = theMesh->nodes->X[map[j]];
        y[j] = theMesh->nodes->Y[map[j]];
    }

# endif
# ifndef NOPOISSONSOLVE

void femPoissonSolve(femPoissonProblem *theProblem)
    femMesh *theMesh = theProblem->geo->theElements;
    femDomain *theBoundary = geoGetDomain(theProblem->geo,"Boundary");
    femFullSystem *theSystem = theProblem->system;
    femIntegration *theRule = theProblem->rule;
    femDiscrete *theSpace = theProblem->space;
 
    if (theSpace->n > 4) Error("Unexpected discrete space size !");  
    double x[4],y[4],phi[4],dphidxsi[4],dphideta[4],dphidx[4],dphidy[4];
    int iElem,iInteg,iEdge,i,j,map[4];
    int nLocal = theMesh->nLocalNode;

    // A completer :-)

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

            //Jacobienne
            double jac = dxdxsi * dydeta - dxdeta * dydxsi;

            //Calcul de Ae_{ij}
            for (i = 0; i < theSpace->n; i++){    
                dphidx[i] = (dphidxsi[i] * dydeta - dphideta[i] * dydxsi) / jac;       
                dphidy[i] = (dphideta[i] * dxdxsi - dphidxsi[i] * dxdeta) / jac; 
            }  

            //Ajout du systeme lineaire local (Assemblage et integration)
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
    
    //Application des contraintes sur les noeuds de la frontiere
    for (iEdge= 0; iEdge < theBoundary->nElem; iEdge++){      
       femFullSystemConstrain(theSystem,theBoundary->elem[iEdge],0.0);  
    }
      
    //Resolution du systeme
    femFullSystemEliminate(theSystem);
}

# endif

