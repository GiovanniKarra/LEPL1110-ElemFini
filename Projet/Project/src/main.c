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
	// geoMeshRead("../data/aximesh.txt");
	// geoMeshRead("../data/test.txt");
	femProblem *theProblem = femElasticityRead(theGeometry, problempath, SOLVER_BAND);
	// femProblem *theProblem = femElasticityRead(theGeometry, "../data/problem.txt", SOLVER_FULL);
	// femProblem *theProblem = femElasticityRead(theGeometry, "../data/problemAXYSIM.txt", SOLVER_FULL);
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
	// femSolutionWrite(nNodes, 2, theSoluce, "../../ProjectPostProcessor/data/UVAXYSIM.txt");
	femElasticityFree(theProblem);
	
	geoFree();


	// int animframes = 60;
	// char path1[30];
	// char path2[50];
	// for (int i = 0; i < animframes; i++) {
	// 	theGeometry = geoGetGeometry();
	// 	geoMeshRead(meshpath);

	// 	sprintf(path1, "../data/anim/frame%d.txt", i);
	// 	sprintf(path2, "../../ProjectPostProcessor/data/anim/UV%d.txt", i);

	// 	theProblem = femElasticityRead(theGeometry, path1, SOLVER_BAND);
	// 	femElasticityPrint(theProblem);

	// 	double *theSoluce = femElasticitySolve(theProblem);
	// 	int nNodes = theGeometry->theNodes->nNodes;
	// 	femSolutionWrite(nNodes, 2, theSoluce, path2);
	// 	femElasticityFree(theProblem);

	// 	printf("path1 : %s\n", path1);
	// 	geoFree();
	// }

	return 0;
}
