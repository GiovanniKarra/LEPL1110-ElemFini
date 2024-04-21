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
#include "glfem.h"

int main(int argc, char **argv) {

	if (argc != 3) Error("Need 2 argument : mesh path and problem path");
	char *meshpath = argv[1];
	char *problempath = argv[2];

	int PRINT = FALSE;

	femGeo *theGeometry = geoGetGeometry();
	geoMeshRead(meshpath);
	femProblem *theProblem = femElasticityRead(theGeometry, problempath, SOLVER_BAND);
	if (PRINT) femElasticityPrint(theProblem);


	// femElasticityAssembleElements(theProblem);
	// GLFWwindow* window = glfemInit("LEPL1110 : Band Solver");
	// glfwMakeContextCurrent(window);
	// while (glfwWindowShouldClose(window) != 1) {
	// 	int w,h;
	// 	glfwGetFramebufferSize(window,&w,&h);
	// 	glColor3f(1.0,0.0,0.0);
	// 	glfemPlotSolver(theProblem->system->band, theProblem->system->band->size,w,h);
	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();
	// }

	double *theSoluce = femElasticitySolve(theProblem);
	int nNodes = theGeometry->theNodes->nNodes;

	char solname[128];
	memcpy(solname, problempath+2, strlen(problempath+2)+1);
	solname[strlen(solname)-4] = '\0';
	strcat(solname, "UV.txt");

	char solpath[256];
	strcat(solpath, "../../ProjectPostProcessor");
	strcat(solpath, solname);
	femSolutionWrite(nNodes, 2, theSoluce, solpath);
	femElasticityFree(theProblem);
	
	geoFree();

	return 0;
}
