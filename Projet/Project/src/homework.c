#include "fem.h"

// Il faut un fifrelin generaliser ce code.....
//    (1) Ajouter l'axisymétrique !        (mandatory)
//    (2) Ajouter les conditions de Neumann !     (mandatory)
//    (3) Ajouter les conditions en normal et tangentiel !     (strongly advised)
//    (4) Et remplacer le solveur plein par un truc un fifrelin plus subtil    (mandatory)

void femElasticityAssembleElements(femProblem *theProblem) {
	femSolverType solverType = theProblem->solverType;
	femSystem *theSystem = theProblem->system;
	femIntegration *theRule = theProblem->rule;
	femDiscrete *theSpace = theProblem->space;
	femGeo *theGeometry = theProblem->geometry;
	femNodes *theNodes = theGeometry->theNodes;
	femMesh *theMesh = theGeometry->theElements;
	double x[4], y[4], phi[4], dphidxsi[4], dphideta[4], dphidx[4], dphidy[4];
	int iElem, iInteg, iEdge, i, j, d, map[4], mapX[4], mapY[4];
	int nLocal = theMesh->nLocalNode;
	double a = theProblem->A;
	double b = theProblem->B;
	double c = theProblem->C;
	double rho = theProblem->rho;
	double gx = theProblem->gx;
	double gy = theProblem->gy;
	double **A = solverType == SOLVER_FULL? theSystem->full->A : theSystem->band->A;
	double *B = solverType == SOLVER_FULL? theSystem->full->B : theSystem->band->B;

	for (iElem = 0; iElem < theMesh->nElem; iElem++) {
		for (j = 0; j < nLocal; j++) {
			map[j] = theMesh->elem[iElem * nLocal + j];
			mapX[j] = 2 * map[j];
			mapY[j] = 2 * map[j] + 1;
			x[j] = theNodes->X[map[j]];
			y[j] = theNodes->Y[map[j]];
		}

		for (iInteg = 0; iInteg < theRule->n; iInteg++) {
			double xsi = theRule->xsi[iInteg];
			double eta = theRule->eta[iInteg];
			double weight = theRule->weight[iInteg];
			femDiscretePhi2(theSpace, xsi, eta, phi);
			femDiscreteDphi2(theSpace, xsi, eta, dphidxsi, dphideta);

			double dxdxsi = 0.0;
			double dxdeta = 0.0;
			double dydxsi = 0.0;
			double dydeta = 0.0;
			for (i = 0; i < theSpace->n; i++) {
				dxdxsi += x[i] * dphidxsi[i];
				dxdeta += x[i] * dphideta[i];
				dydxsi += y[i] * dphidxsi[i];
				dydeta += y[i] * dphideta[i];
			}
			double jac = dxdxsi * dydeta - dxdeta * dydxsi;
			if (jac < 0.0)
				printf("Negative jacobian! Your mesh is oriented in reverse. The normals will be wrong\n");
			jac = fabs(jac);

			for (i = 0; i < theSpace->n; i++) {
				dphidx[i] = (dphidxsi[i] * dydeta - dphideta[i] * dydxsi) / jac;
				dphidy[i] = (dphideta[i] * dxdxsi - dphidxsi[i] * dxdeta) / jac;
			}
			for (i = 0; i < theSpace->n; i++) {
				for (j = 0; j < theSpace->n; j++) {
					A[mapX[i]][mapX[j]] += (dphidx[i] * a * dphidx[j] + dphidy[i] * c * dphidy[j]) * jac * weight;
					A[mapX[i]][mapY[j]] += (dphidx[i] * b * dphidy[j] + dphidy[i] * c * dphidx[j]) * jac * weight;
					A[mapY[i]][mapX[j]] += (dphidy[i] * b * dphidx[j] + dphidx[i] * c * dphidy[j]) * jac * weight;
					A[mapY[i]][mapY[j]] += (dphidy[i] * a * dphidy[j] + dphidx[i] * c * dphidx[j]) * jac * weight;
				}
			}
			for (i = 0; i < theSpace->n; i++) {
				B[mapX[i]] += phi[i] * gx * rho * jac * weight;
				B[mapY[i]] += phi[i] * gy * rho * jac * weight;
			}
		}
	}
}

void femElasticityAssembleNeumann(femProblem *theProblem) {
	femSystem *theSystem = theProblem->system;
	femSolverType solverType = theProblem->solverType;
	femIntegration *theRule = theProblem->ruleEdge;
	femDiscrete *theSpace = theProblem->spaceEdge;
	femGeo *theGeometry = theProblem->geometry;
	femNodes *theNodes = theGeometry->theNodes;
	femMesh *theEdges = theGeometry->theEdges;
	double x[2], y[2], phi[2];
	int iBnd, iElem, iInteg, iEdge, i, j, d, map[2];
	int nLocal = 2;
	double *B = solverType == SOLVER_FULL? theSystem->full->B : theSystem->band->B;

	for (iBnd = 0; iBnd < theProblem->nBoundaryConditions; iBnd++) {
		femBoundaryCondition *theCondition = theProblem->conditions[iBnd];
		femBoundaryType type = theCondition->type;
		double value = theCondition->value1;

		if(type != NEUMANN_X && type != NEUMANN_Y && type != NEUMANN_N && type != NEUMANN_T){
			continue;
		}

		for (iEdge = 0; iEdge < theCondition->domain->nElem; iEdge++) {
			iElem = theCondition->domain->elem[iEdge];
			for (j = 0; j < nLocal; j++) {
				map[j] = theEdges->elem[iElem * nLocal + j];
				x[j] = theNodes->X[map[j]];
				y[j] = theNodes->Y[map[j]];
			}

			double tx = x[1] - x[0];
			double ty = y[1] - y[0];
			double length = hypot(tx, ty);
			double jac = length / 2.0;
			
			double f_x = 0.0;
			double f_y = 0.0;
			if (type == NEUMANN_X) {
				f_x = value;
			}
			if (type == NEUMANN_Y) {
				f_y = value;
			}
			if (type == NEUMANN_T) {
				double t_x = tx / length;
				double t_y = ty / length;

				f_x = value*t_x;
				f_y = value*t_y;
			}
			if (type == NEUMANN_N) {
				double ny = -tx / length;
				double nx = ty / length;

				f_x = value*nx;
				f_y = value*ny;
			}

			//
			// A completer :-)
			// Attention, pour le normal tangent on calcule la normale (sortante) au SEGMENT, surtout PAS celle de constrainedNodes
			// Une petite aide pour le calcul de la normale :-)
			// double nx =    ty / length;
			// double ny = -tx / length;

			for (iInteg = 0; iInteg < theRule->n; iInteg++) {
				double xsi = theRule->xsi[iInteg];
				double weight = theRule->weight[iInteg];
				femDiscretePhi(theSpace, xsi, phi);
				for (i = 0; i < theSpace->n; i++) {
					B[2*map[i] + 0] += jac * weight * phi[i] * f_x;
					B[2*map[i] + 1] += jac * weight * phi[i] * f_y;
				}
			}
		}
	}
}

void femElasticityApplyDirichlet(femProblem *theProblem) {
	femSystem *theSystem = theProblem->system;
	femGeo *theGeometry = theProblem->geometry;
	femNodes *theNodes = theGeometry->theNodes;

	for (int node = 0; node < theNodes->nNodes; node++) {
		femConstrainedNode *theConstrainedNode = &theProblem->constrainedNodes[node];
		if (theConstrainedNode->type == UNDEFINED)
			continue;
		femBoundaryType type = theConstrainedNode->type;

		if (type == DIRICHLET_X) {
			double value = theConstrainedNode->value1;
			femFullSystemConstrain(theSystem->full, 2 * node + 0, value);
		}
		if (type == DIRICHLET_Y) {
			double value = theConstrainedNode->value1;
			femFullSystemConstrain(theSystem->full, 2 * node + 1, value);
		}
		if (type == DIRICHLET_XY) {
			double value_x = theConstrainedNode->value1;
			double value_y = theConstrainedNode->value2;
			femFullSystemConstrain(theSystem->full, 2 * node + 0, value_x);
			femFullSystemConstrain(theSystem->full, 2 * node + 1, value_y);
		}

		if (type == DIRICHLET_N) {
			double value = theConstrainedNode->value1;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			femFullSystemConstrain(theSystem->full, 2 * node + 0, value*nx);
			femFullSystemConstrain(theSystem->full, 2 * node + 1, value*ny);
		}
		if (type == DIRICHLET_T) {
			double value = theConstrainedNode->value1;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			double tx = ny;
			double ty = -nx;
			femFullSystemConstrain(theSystem->full, 2 * node + 0, value*tx);
			femFullSystemConstrain(theSystem->full, 2 * node + 1, value*ty);
		}
		if (type == DIRICHLET_NT) {
			double value_n = theConstrainedNode->value1;
			double value_t = theConstrainedNode->value2;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			// A completer :-)
		}
	}
}


double *femElasticitySolve(femProblem *theProblem) {
	femElasticityAssembleElements(theProblem);
	femElasticityAssembleNeumann(theProblem);
	femElasticityApplyDirichlet(theProblem);

	double *soluce;
	if (theProblem->solverType == SOLVER_FULL)
		soluce = femFullSystemEliminate(theProblem->system->full);
	else if (theProblem->solverType == SOLVER_BAND)
		soluce = femBandSystemEliminate(theProblem->system->band);

	memcpy(theProblem->soluce, soluce, theProblem->system->full->size * sizeof(double));
	return theProblem->soluce;
}
