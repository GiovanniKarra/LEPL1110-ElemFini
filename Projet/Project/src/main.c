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

int main(void) {
	femGeo *theGeometry = geoGetGeometry();
	geoMeshRead("../data/mesh.txt");
	// geoMeshRead("../data/aximesh.txt");
	// geoMeshRead("../data/test.txt");
	femProblem *theProblem = femElasticityRead(theGeometry, "../data/problem.txt", SOLVER_BAND);
	// femProblem *theProblem = femElasticityRead(theGeometry, "../data/problem.txt", SOLVER_FULL);
	// femProblem *theProblem = femElasticityRead(theGeometry, "../data/problemAXYSIM.txt", SOLVER_FULL);
	femElasticityPrint(theProblem);


	// femElasticityAssembleElements(theProblem);
	// GLFWwindow* window = glfemInit("LEPL1110 : Band Solver");
	// glfwMakeContextCurrent(window);
	// while (glfwWindowShouldClose(window) != 1) {
	// 	int w,h;
    //     glfwGetFramebufferSize(window,&w,&h);
	// 	glColor3f(1.0,0.0,0.0);
	// 	glfemPlotSolver(theProblem->system->band, theProblem->system->band->size,w,h);
	// 	glfwSwapBuffers(window);
    //     glfwPollEvents();
	// }

	double *theSoluce = femElasticitySolve(theProblem);
	int nNodes = theGeometry->theNodes->nNodes;
	femSolutionWrite(nNodes, 2, theSoluce, "../../ProjectPostProcessor/data/UV.txt");
	// femSolutionWrite(nNodes, 2, theSoluce, "../../ProjectPostProcessor/data/UVAXYSIM.txt");
	femElasticityFree(theProblem);

	// int animframes = 20;
	// char path1[25];
	// char path2[46];
	// for (int i = 0; i < animframes; i++) {
	// 	sprintf(path1, "../data/anim/frame%d.txt", i);
	// 	sprintf(path2, "../../ProjectPostProcessor/data/anim/UV%d.txt", i);

	// 	theProblem = femElasticityRead(theGeometry, path1, SOLVER_BAND);
	// 	femElasticityPrint(theProblem);

	// 	double *theSoluce = femElasticitySolve(theProblem);
	// 	int nNodes = theGeometry->theNodes->nNodes;
	// 	femSolutionWrite(nNodes, 2, theSoluce, path2);
	// 	femElasticityFree(theProblem);

	// 	printf("i : %d\n", i);
	// }
	geoFree();

	return 0;
}
