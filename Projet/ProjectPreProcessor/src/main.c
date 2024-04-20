/*
 *  main.c
 *  Projet 2022-2023
 *  Elasticite lineaire plane
 *
 *  Preprocesseur
 *
 *  Copyright (C) 2023 UCL-IMMC : Vincent Legat
 *  All rights reserved.
 *
 */

#include "glfem.h"

int main(int argc, char **argv)
{
	if (argc != 2) Error("Need 1 argument : mesh name");
	char *meshname = argv[1];
	char meshpath[strlen(meshname)+13];
	sprintf(meshpath, "../data/%s.msh", meshname);

	//
	//  -1- Construction de la geometrie
	//
	geoInitialize();
	femGeo *theGeometry = geoGetGeometry();
	// geoMeshRead("../data/aximesh.txt");
	geoMeshGenerateMshFile(meshpath);

	geoMeshImport();
	geoSetDomainName(0, "RoueInterne1");
	geoSetDomainName(1, "RoueInterne2");
	geoSetDomainName(2, "RoueExterne1");
	geoSetDomainName(3, "RoueExterne2");

	char projectmesh[24+strlen(meshname)];
	sprintf(projectmesh, "../../Project/data/%s.txt", meshname);
	char postmesh[37+strlen(meshname)];
	sprintf(postmesh, "../../ProjectPostProcessor/data/%s.txt", meshname);

	geoMeshWrite(projectmesh);
	geoMeshWrite(postmesh);

	//
	//  -2- Definition du probleme
	//

	double E = 211.e9;
	double nu = 0.3;
	double rho = 800;
	double gx = 0;
	double gy = -9.81;

	femProblem *theProblem = femElasticityCreate(theGeometry, E, nu, rho, gx, gy, PLANAR_STRAIN);
	femElasticityAddBoundaryCondition(theProblem, "RoueInterne1", DIRICHLET_XY, 0.0, 0.0);
	femElasticityAddBoundaryCondition(theProblem, "RoueInterne2", DIRICHLET_XY, 0.0, 0.0);
	femElasticityAddBoundaryCondition(theProblem, "RoueExterne2", NEUMANN_T, -2000.0, NAN);
	femElasticityPrint(theProblem);
	femElasticityWrite(theProblem, "../../Project/data/problem.txt");
	femElasticityWrite(theProblem, "../../ProjectPostProcessor/data/problem.txt");
	
	// geoSetDomainName(0,"Symmetry");
	// geoSetDomainName(1,"Top");
    // geoSetDomainName(7,"Bottom");
	// double E   = 211.e9;
    // double nu  = 0.3;
    // double rho = 7.85e3; 
    // double g   = 9.81;
    // femProblem* theProblem = femElasticityCreate(theGeometry,E,nu,rho,0, -g,PLANAR_STRAIN);
    // femElasticityAddBoundaryCondition(theProblem,"Symmetry",DIRICHLET_X,0.0, NAN);
    // femElasticityAddBoundaryCondition(theProblem,"Bottom",DIRICHLET_Y,0.0, NAN);
    // femElasticityAddBoundaryCondition(theProblem,"Top",NEUMANN_Y,-1e4,NAN);
	// femElasticityPrint(theProblem);
	// femElasticityWrite(theProblem, "../../Project/data/problem.txt");
	// femElasticityWrite(theProblem, "../../ProjectPostProcessor/data/problem.txt");

	// char path1[40];
	// char path2[50];
	// int frameCount = 60;
	// femProblem *theProblem;
	// for (int i = 0; i < frameCount; i++) {
	// 	printf("%d\n", i);
	// 	theProblem = femElasticityCreate(theGeometry, E, nu, rho, gx, gy, PLANAR_STRAIN);
	// 	femElasticityAddBoundaryCondition(theProblem, "RoueInterne1", DIRICHLET_Y, 0.0, NAN);
	// 	femElasticityAddBoundaryCondition(theProblem, "RoueInterne2", DIRICHLET_XY, 0.0, 0.0);
	// 	// femElasticityAddBoundaryCondition(theProblem, "RoueExterne2", NEUMANN_N, -500.0, NAN);
	// 	femElasticityAddBoundaryCondition(theProblem, "RoueExterne2", NEUMANN_T, -5000.0*i/frameCount, NAN);
	// 	femElasticityPrint(theProblem);

	// 	sprintf(path1, "../../Project/data/anim/frame%d.txt", i);
	// 	sprintf(path2, "../../ProjectPostProcessor/data/anim/frame%d.txt", i);

	// 	femElasticityWrite(theProblem, path1);
	// 	femElasticityWrite(theProblem, path2);
	// 	femElasticityFree(theProblem);
	// }

	//
	//  -3- Champ de la taille de référence du maillage (uniquement pour la visualisation)
	//

	// double *meshSizeField = malloc(theGeometry->theNodes->nNodes * sizeof(double));
	// femNodes *theNodes = theGeometry->theNodes;
	// for (int i = 0; i < theNodes->nNodes; ++i)
	// 	meshSizeField[i] = theGeometry->geoSize(theNodes->X[i], theNodes->Y[i]);
	// double hMin = femMin(meshSizeField, theNodes->nNodes);
	// double hMax = femMax(meshSizeField, theNodes->nNodes);
	// printf(" ==== Global requested h : %14.7e \n", theGeometry->h);
	// printf(" ==== Minimum h          : %14.7e \n", hMin);
	// printf(" ==== Maximum h          : %14.7e \n", hMax);

	//
	//  -4- Visualisation
	//

	// int mode = 1;
	// int domain = 0;
	// int freezingButton = FALSE;
	// double t, told = 0;
	// char theMessage[MAXNAME];

	// GLFWwindow *window = glfemInit("EPL1110 : Project 2022-23 ");
	// glfwMakeContextCurrent(window);
	// glfwSetScrollCallback(window, scroll_callback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);

	// do
	// {
	// 	int w, h;
	// 	glfwGetFramebufferSize(window, &w, &h);
	// 	glfemReshapeWindows(window, theGeometry->theNodes, w, h);

	// 	t = glfwGetTime();
	// 	if (glfwGetKey(window, 'D') == GLFW_PRESS)
	// 	{
	// 		mode = 0;
	// 	}
	// 	if (glfwGetKey(window, 'V') == GLFW_PRESS)
	// 	{
	// 		mode = 1;
	// 	}
	// 	if (glfwGetKey(window, 'N') == GLFW_PRESS && freezingButton == FALSE)
	// 	{
	// 		domain++;
	// 		freezingButton = TRUE;
	// 		told = t;
	// 	}

	// 	if (t - told > 0.5)
	// 	{
	// 		freezingButton = FALSE;
	// 	}
	// 	if (mode == 1)
	// 	{
	// 		glfemPlotField(theGeometry->theElements, meshSizeField);
	// 		glfemPlotMesh(theGeometry->theElements);
	// 		sprintf(theMessage, "Number of elements : %d ", theGeometry->theElements->nElem);
	// 		glColor3f(1.0, 0.0, 0.0);
	// 		glfemMessage(theMessage);
	// 	}
	// 	if (mode == 0)
	// 	{
	// 		domain = domain % theGeometry->nDomains;
	// 		glfemPlotDomain(theGeometry->theDomains[domain]);
	// 		sprintf(theMessage, "%s : %d ", theGeometry->theDomains[domain]->name, domain);
	// 		glColor3f(1.0, 0.0, 0.0);
	// 		glfemMessage(theMessage);
	// 	}

	// 	glfwSwapBuffers(window);
	// 	glfwPollEvents();
	// } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) != 1);


	// glfwTerminate();
	// free(meshSizeField);
	femElasticityFree(theProblem);
	geoFree();

	exit(EXIT_SUCCESS);
	return 0;
}
