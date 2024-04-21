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
	int *number = theMesh->nodes->number;
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
			mapX[j] = 2 * number[map[j]];
			mapY[j] = mapX[j] + 1;
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
			double r = 1.0;

			for (i = 0; i < theSpace->n; i++) {
				dphidx[i] = (dphidxsi[i] * dydeta - dphideta[i] * dydxsi) / jac;
				dphidy[i] = (dphideta[i] * dxdxsi - dphidxsi[i] * dxdeta) / jac;
			}

			if (theProblem->planarStrainStress != AXISYM) {
				for (i = 0; i < theSpace->n; i++) {
					for (j = 0; j < theSpace->n; j++) {
						// printf("%d, %d\n", mapX[i], mapX[j]);
						A[mapX[i]][mapX[j]] += (dphidx[i] * a * dphidx[j] + dphidy[i] * c * dphidy[j]) * jac * weight;
						A[mapX[i]][mapY[j]] += (dphidx[i] * b * dphidy[j] + dphidy[i] * c * dphidx[j]) * jac * weight;
						A[mapY[i]][mapX[j]] += (dphidy[i] * b * dphidx[j] + dphidx[i] * c * dphidy[j]) * jac * weight;
						A[mapY[i]][mapY[j]] += (dphidy[i] * a * dphidy[j] + dphidx[i] * c * dphidx[j]) * jac * weight;
					}
				}
			}
			else {
				r = 0.0;
				for (i = 0; i < theSpace->n; i++) r += x[i] * phi[i];

				for (i = 0; i < theSpace->n; i++) {
					for (j = 0; j < theSpace->n; j++) {
						A[mapX[i]][mapX[j]] += (dphidx[i] * a * r * dphidx[j] + dphidy[i] * c * r * dphidy[j] + phi[i] * (b * dphidx[j] + a * phi[j] / r) + dphidx[i] * phi[j] * b) * jac * weight;
						A[mapX[i]][mapY[j]] += (dphidx[i] * b * dphidy[j] + dphidy[i] * c * dphidx[j]) * jac * weight * r + phi[i] * b * dphidy[j] * jac * weight;
						A[mapY[i]][mapX[j]] += (dphidy[i] * b * dphidx[j] + dphidx[i] * c * dphidy[j]) * jac * weight * r + phi[j] * b * dphidy[i] * jac * weight;
						A[mapY[i]][mapY[j]] += (dphidy[i] * a * dphidy[j] + dphidx[i] * c * dphidx[j]) * jac * weight * r;				
					}
				}
			}
			for (i = 0; i < theSpace->n; i++) {
				B[mapX[i]] += phi[i] * gx * rho * jac * weight * r;
				B[mapY[i]] += phi[i] * gy * rho * jac * weight * r;
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
	int *number = theGeometry->theElements->nodes->number;
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

			double axim_x = 1.0;
			double axim_y = 1.0;
			if (theProblem->planarStrainStress == AXISYM) {
				double xsi2[2] = {-1 / sqrt(3), 1 / sqrt(3)};
				double phi1[2] = {(-xsi2[0] + 1) / 2, (-xsi2[1] + 1) / 2};
				double phi2[2] = {(xsi2[0] + 1) / 2, (xsi2[1] + 1) / 2};

				axim_x = x[0] * phi1[0] + x[1] * phi2[0];
				axim_y = x[0] * phi1[1] + x[1] * phi2[1];
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
					B[2*number[map[i]] + 0] += jac * weight * phi[i] * f_x * axim_x;
					B[2*number[map[i]] + 1] += jac * weight * phi[i] * f_y * axim_y;
				}
			}
		}
	}
}

void femElasticityApplyDirichlet(femProblem *theProblem) {
	femSystem *theSystem = theProblem->system;
	femSolverType solverType = theProblem->solverType;
	femGeo *theGeometry = theProblem->geometry;
	femNodes *theNodes = theGeometry->theNodes;
	int *number = theGeometry->theElements->nodes->number;

	for (int node = 0; node < theNodes->nNodes; node++) {
		femConstrainedNode *theConstrainedNode = &theProblem->constrainedNodes[node];
		if (theConstrainedNode->type == UNDEFINED)
			continue;
		femBoundaryType type = theConstrainedNode->type;

		if (type == DIRICHLET_X) {
			double value = theConstrainedNode->value1;
			if (solverType == SOLVER_FULL)
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value);
			else if (solverType == SOLVER_BAND)
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value);
		}
		if (type == DIRICHLET_Y) {
			double value = theConstrainedNode->value1;
			if (solverType == SOLVER_FULL)
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value);
			else if (solverType == SOLVER_BAND)
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value);
		}
		if (type == DIRICHLET_XY) {
			double value_x = theConstrainedNode->value1;
			double value_y = theConstrainedNode->value2;
			if (solverType == SOLVER_FULL) {
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value_x);
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value_y);
			}
			else if (solverType == SOLVER_BAND) {
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value_x);
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value_y);
			}
		}

		if (type == DIRICHLET_N) {
			double value = theConstrainedNode->value1;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			double tx = -ny;
			double ty = nx;
			if (solverType == SOLVER_FULL) {
				for (int i = 0; i < theSystem->full->size; i++) {
					theSystem->full->A[i][2 * node + 0] = tx*(tx*theSystem->full->A[i][2 * node + 0] + ty*theSystem->full->A[i][2 * node + 1]);
					theSystem->full->A[i][2 * node + 1] = ty*(tx*theSystem->full->A[i][2 * node + 0] + ty*theSystem->full->A[i][2 * node + 1]);

					theSystem->full->B[i] -= theSystem->full->A[i][2 * node + 0] * value * nx + theSystem->full->A[i][2 * node + 1] * value * ny;
				}
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value*nx);
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value*ny);
			}
			else if (solverType == SOLVER_BAND) {
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value*nx);
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value*ny);
			}
		}
		if (type == DIRICHLET_T) {
			double value = theConstrainedNode->value1;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			double tx = ny;
			double ty = -nx;
			if (solverType == SOLVER_FULL) {
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value*tx);
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value*ty);
			}
			else if (solverType == SOLVER_BAND) {
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value*tx);
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value*ty);
			}
		}
		if (type == DIRICHLET_NT) {
			double value_n = theConstrainedNode->value1;
			double value_t = theConstrainedNode->value2;
			double nx = theConstrainedNode->nx;
			double ny = theConstrainedNode->ny;
			double tx = ny;
			double ty = -nx;
			if (solverType == SOLVER_FULL) {
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value_n*nx);
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value_n*ny);

				femFullSystemConstrain(theSystem->full, 2 * number[node] + 0, value_t*tx);
				femFullSystemConstrain(theSystem->full, 2 * number[node] + 1, value_t*ty);
			}
			else if (solverType == SOLVER_BAND) {
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value_n*nx);
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value_n*ny);

				femBandSystemConstrain(theSystem->band, 2 * number[node] + 0, value_t*tx);
				femBandSystemConstrain(theSystem->band, 2 * number[node] + 1, value_t*ty);
			}
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

	int *number = theProblem->geometry->theElements->nodes->number;
	int nNodes = theProblem->geometry->theElements->nodes->nNodes;
	for (int i = 0; i < nNodes; i++) {
		theProblem->soluce[2*i] = soluce[2*number[i]];
		theProblem->soluce[i*2+1] = soluce[2*number[i]+1];
	}
	return theProblem->soluce;
}
