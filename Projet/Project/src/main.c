/*
 *    main.c
 *    Projet 2022-2023
 *    Elasticite lineaire plane
 *
 *    Code de calcul
 *
 *    Copyright (C) 2023 UCL-IMMC : Vincent Legat
 *    All rights reserved.
 *
 */

#include "fem.h"
// #include "glfem.h"

int main(void) {
	femGeo *theGeometry = geoGetGeometry();
	geoMeshRead("../data/mesh.txt");
	// femProblem *theProblem = femElasticityRead(theGeometry, "../data/problem.txt", SOLVER_FULL);
	femProblem *theProblem = femElasticityRead(theGeometry, "../data/problem.txt", SOLVER_FULL);
	femElasticityPrint(theProblem);

	// GLFWwindow* window = glfemInit("LEPL1110 : Band Solver ");
	// glfwMakeContextCurrent(window);
	// glColor3f(1.0,0.0,0.0);
	// glfemPlotSolver(theProblem->system->band, theProblem->system->band->size,500,500);

	double *theSoluce = femElasticitySolve(theProblem);
	int nNodes = theGeometry->theNodes->nNodes;
	femSolutionWrite(nNodes, 2, theSoluce, "../../ProjectPostProcessor/data/UV.txt");
	femElasticityFree(theProblem);
	geoFree();
	return 0;
}
