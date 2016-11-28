#include "Brep.h"
#include "controls.hpp"
#include "EularOperation.h"
#include "filereader.h"

GLFWwindow *window;
#include <stdio.h>

int main()
{
	/*************************************************************/
	//init opengl context
	/*************************************************************/
	if( !glfwInit() )
	{
		fprintf(stderr, "failed to init glwf!");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	window = glfwCreateWindow(1024, 768, "triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0, 0, 0.4, 1.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	/*************************************************************/
	//create program and get location of attrib and uniform
	/*************************************************************/
	
	GLuint programID = LoadShaders( "shader/vs", "shader/fs" );
	GLuint vertex_loc = glGetAttribLocation(programID, "vertexPosition");
	GLuint normal_loc = glGetAttribLocation(programID, "vertexNormal");

	GLuint mvpID = glGetUniformLocation(programID, "MVP");
	GLuint modelID = glGetUniformLocation(programID, "M");
	/*************************************************************/
	//send transform matrix
	/*************************************************************/

	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	glm::mat4 Model      = glm::mat4(1.0f);
	glm::mat4 MVP        = Projection * View * Model; 

	/*************************************************************/
	//create entity (test)
	/*************************************************************/
	/*Vec3f vertices[16];
	vertices[0] = Vec3f(-1, -1, 0);
	vertices[1] = Vec3f(-1, 1, 0);
	vertices[2] = Vec3f(1, 1, 0);
	vertices[3] = Vec3f(1, -1, 0);*/

	/*vertices[4] = Vec3f(-1, -1, 2);
	vertices[5] = Vec3f(1, -1, 2);
	vertices[6] = Vec3f(1, 1, 2);
	vertices[7] = Vec3f(-1, 1, 2);

	vertices[8] = Vec3f(0.5, 0.5, 2);
	vertices[9] = Vec3f(-0.5, 0.5, 2);
	vertices[10] = Vec3f(-0.5, -0.5, 2);
	vertices[11] = Vec3f(0.5, -0.5, 2);*/

	/*vertices[12] = Vec3f(0.5, 0.5, 0);
	vertices[13] = Vec3f(-0.5, 0.5, 0);
	vertices[14] = Vec3f(-0.5, -0.5, 0);
	vertices[15] = Vec3f(0.5, -0.5, 0);*/

	/*---------------------------------------------------------------*/
	//initialize: generate a vertex, an outter loop, a face, a solid
	/*Solid *solid = mvsf(vertices[0]);
	Loop *loop = solid->getFirstFace()->getOuterLoop();
	Vertex *first_v = loop->getFirstVertex(), *v = first_v;*/

	Solid *solid;
	FileReader fr;
	Face *f = fr.readFromFile("face.txt", solid);

	float len = fr.getLen();
	Vec3f dir = fr.getDir();
	/*---------------------------------------------------------------*/
	//generate bottom face
	//for(int i=1; i<4; ++i)
	//{
	//	HalfEdge *hf = mev(v, loop, vertices[i]);
	//	v = hf->getEndV();
	//}
	//
	//Face *f = mef(v, first_v, loop);
	//solid->addFace(f);
	//
	///*---------------------------------------------------------------*/
	////generate inside face
	//HalfEdge *bridge = mev(f->getOuterLoop()->getFirstEdge()->getStartV(), f->getOuterLoop(), vertices[12]);
	//Loop *inner_loop = kemr(f->getOuterLoop(), bridge);
	//first_v = bridge->getEndV(), v = first_v;

	//for(int i=13; i<16; ++i)
	//{
	//	HalfEdge *hf = mev(v, inner_loop, vertices[i]);
	//	v = hf->getEndV();
	//}
	//
	//Face *inside_f = mef(v, first_v, inner_loop);
	////inside_f->printFace();
	//solid->addFace(inside_f);

	sweep(f, len, dir);
	int num = solid->getFaceNum();
	solid->printfFaces();

	GLfloat *vertexPosition, *normals;
	GLuint v_num = solid->printfVerticesIntoBuffer(vertexPosition, normals);
	solid->printStartIdx();
	/*************************************************************/
	//generate and bind buffers
	/*************************************************************/

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, v_num * 3 *sizeof(GLfloat), vertexPosition, GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, v_num * 3 *sizeof(GLfloat), normals, GL_STATIC_DRAW);


	///*************************************************************/
	////render
	///*************************************************************/

	do
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUseProgram(programID);
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);

		glEnableVertexAttribArray(vertex_loc);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, 0, 0, 0);

		glEnableVertexAttribArray(normal_loc);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, 0, 0, 0);

		
		/*
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilMask(0xFF);

		glDrawArrays(GL_QUADS, 4, 8);*/

		/*glStencilFunc(GL_EQUAL, 0, 0xFF);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

		glDrawArrays(GL_QUADS, 0, 4);*/


		/*glStencilFunc(GL_EQUAL, 0, 0xFF);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDrawArrays(GL_QUADS, 0, 4);*/

		//glDrawArrays(GL_QUADS, 0, v_num);
		solid->drawFaces();

		glDisableVertexAttribArray(vertex_loc);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);

	glfwTerminate();

	return 0;
}