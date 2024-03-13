

#include"fem.h"


#ifndef NORENUMBER 

double *GlobalArray;

int compare_pos(const void *a, const void *b) {
    int *ia = (int*)a; int *ib = (int*)b;
    double diff = GlobalArray[*ia] - GlobalArray[*ib];

    return (diff > 0) - (diff < 0);
}

void femMeshRenumber(femMesh *theMesh, femRenumType renumType)
{
    int i;

    int *inverse = (int*)malloc(sizeof(int)*theMesh->nodes->nNodes);
    for (i = 0; i < theMesh->nodes->nNodes; i++)
        inverse[i] = i;
    
    switch (renumType) {
        case FEM_NO :
            for (i = 0; i < theMesh->nodes->nNodes; i++) 
                theMesh->nodes->number[i] = i;
            break;
// 
// A modifier :-)
// debut
//
        case FEM_XNUM :
            GlobalArray = theMesh->nodes->X;
            qsort(inverse, theMesh->nodes->nNodes, sizeof(int), compare_pos);
            break;
        case FEM_YNUM : 
            GlobalArray = theMesh->nodes->Y;
            qsort(inverse, theMesh->nodes->nNodes, sizeof(int), compare_pos);
            break;            
// 
// end
//

        default : Error("Unexpected renumbering option");
    }

    for (i = 0; i < theMesh->nodes->nNodes; i++)
        theMesh->nodes->number[inverse[i]] = i;

    free(inverse);
}

#endif
#ifndef NOBAND 

int femMeshComputeBand(femMesh *theMesh)
{
    int myMax, myMin, myBand, map[4];
    int nLocal = theMesh->nLocalNode;
    myBand = 0;

    for (int i = 0; i < theMesh->nElem; i++) {
        for (int j = 0; j < nLocal; ++ j )
            map[j] = theMesh->nodes->number[theMesh->elem[i*nLocal+j]];

        // On trouve le noeud maximum et minimum
        myMin = map[0];
        myMax = map[0];
        for (int j = 1; j < nLocal ; j++) {
            myMax = map[j] > myMax ? map[j] : myMax;
            myMin = map[j] < myMin ? map[j] : myMin;
        }

        if (myBand < (myMax - myMin))
            myBand = myMax - myMin;
    }

    return myBand+1;
}


#endif
#ifndef NOBANDASSEMBLE


void femBandSystemAssemble(femBandSystem* myBandSystem, double *Aloc, double *Bloc, int *map, int nLoc)
{
    int i,j;
    for (i = 0; i < nLoc; i++) {
        int row = map[i]; 
        for(j = 0; j < nLoc; j++) {
            int col = map[j];
            if (col >= row) {
                myBandSystem->A[row][col] += Aloc[i*nLoc+j];
            }
        }
        myBandSystem->B[map[i]] += Bloc[i];
    }
}


#endif
#ifndef NOBANDELIMINATE


double  *femBandSystemEliminate(femBandSystem *myBand)
{
    double  **A, *B, factor;
    int     i, j, k, jend, size, band;
    A    = myBand->A;
    B    = myBand->B;
    size = myBand->size;
    band = myBand->band;
    
    for (k=0; k < size; k++) {
        jend = k+band < size ? k+band : size;
        for (i = k+1 ; i <  jend; i++) {
            factor = A[k][i] / A[k][k];
            for (j = i ; j < jend; j++) 
                A[i][j] = A[i][j] - A[k][j] * factor;
            B[i] = B[i] - B[k] * factor;
        }
    }
    
    for (i = size-1; i >= 0 ; i--) {
        factor = 0;
        jend = i+band < size ? i+band : size;
        for (j = i+1 ; j < jend; j++)
            factor += A[i][j] * B[j];
        B[i] = (B[i] - factor)/A[i][i];
    }


    return(myBand->B);
}


#endif

