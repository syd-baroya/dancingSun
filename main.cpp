/*
ZJ Wood CPE 471 Lab 3 base code - includes use of glee
https://github.com/nshkurkin/glee
*/

/*
    * sun rays are triangles
         * have to know how many vert you have
         * have to know how many triangles you want
         * for loop, { a = ; b = ; c = ; and put in an array}
         * vec a; a.x = sin(w); a.y = cos(w); // to find the points on the circumference of the triangle
 
     * make circle out of flat triangles
 
*/

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#define PI 3.14159265
//#define POINTS 90
#define IN_RADIUS 0.4
#define OUT_RADIUS 1.0
#define TRI 10


using namespace std;
using namespace glm;

int pixW, pixH;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog, prog2; //our shader program

/* Global data associated with triangle geometry - this will likely vary
   in later programs - so is left explicit for now  */
GLuint VertexArrayID, VertexArrayID2;

GLfloat background_buffer_data[6*3];
GLfloat sun_buffer_data[2*(TRI*9)*3 + (TRI*9)*3];

//GLint indices[TRI*9*2 + TRI*9];


//data necessary to give our triangle data to OGL
GLuint VertexBufferID1, VertexBufferID2;
//GLuint IndexBufferID1;


float paraXCoord(float angle, float radius){
    return radius*cos(angle*PI/180);
}

float paraYCoord(float angle, float radius){
    return radius*sin(angle*PI/180);
}

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

////callback for the mouse when clicked move the triangle when helper functions
////written
//static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
//{
//    double posX, posY;
//    float newPt[2];
//    if (action == GLFW_PRESS) {
//        glfwGetCursorPos(window, &posX, &posY);
//        cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
//        //change this to be the points converted to WORLD
//        //THIS IS BROKEN< YOU GET TO FIX IT - yay!
//        newPt[0] = 0;
//        newPt[1] = 0;
//        cout << "converted:" << newPt[0] << " " << newPt[1] << endl;
//        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID1);
//        //update the vertex array with the updated points
//        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//    }
//}

//if the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int in_width, int in_height) {
	//get the window size - may be different then pixels for retina	
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glViewport(0, 0, width, height);
}



void buildRays(){
    int counter = 0;
    //    float offset = 0.2;
    float x , y , z = 0.0;
    for(int i=0; i<(TRI*9);i++){
        
        if(i%2==0){
            x = paraXCoord(counter*(360/(TRI*9))/2,OUT_RADIUS);
            y = paraYCoord(counter*(360/(TRI*9))/2,OUT_RADIUS);
        }
        else{
            x = paraXCoord(counter*(360/(TRI*9))/2,IN_RADIUS);
            y = paraYCoord(counter*(360/(TRI*9))/2,IN_RADIUS);
        }
        
        sun_buffer_data[i*9] = x;
        sun_buffer_data[i*9+1] = y;
        sun_buffer_data[i*9+2] = z;
        
        counter-=2;
        
        sun_buffer_data[i*9+3] = 0.0;
        sun_buffer_data[i*9+4] = 0.0;
        sun_buffer_data[i*9+5] = 0.0;

        if(i%2==0){
            x = paraXCoord(counter*(360/(TRI*9))/2,IN_RADIUS);
            y = paraYCoord(counter*(360/(TRI*9))/2, IN_RADIUS);
        }
        else{
            x = paraXCoord(counter*(360/(TRI*9))/2,OUT_RADIUS);
            y = paraYCoord(counter*(360/(TRI*9))/2,OUT_RADIUS);
        }

        sun_buffer_data[i*9+6] = x;
        sun_buffer_data[i*9+7] = y;
        sun_buffer_data[i*9+8] = z;
        

    }
    
    
}


void buildBackground(){
    background_buffer_data[0] = -1;
    background_buffer_data[1] = -1;
    background_buffer_data[2] = 0;
    background_buffer_data[3] = -1;
    background_buffer_data[4] = 1;
    background_buffer_data[5] = 0;
    background_buffer_data[6] = 1;
    background_buffer_data[7] = -1;
    background_buffer_data[8] = 0;
    background_buffer_data[9] = 1;
    background_buffer_data[10] = -1;
    background_buffer_data[11] = 0;
    background_buffer_data[12] = -1;
    background_buffer_data[13] = 1;
    background_buffer_data[14] = 0;
    background_buffer_data[15] = 1;
    background_buffer_data[16] = 1;
    background_buffer_data[17] = 0;
}

/*Note that any gl calls must always happen after a GL state is initialized */
static void initGeom() {

	//generate the VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
    
//    buildSun();
//    buildCircle();
    
    buildBackground();

	//generate vertex buffer to hand off to OGL
	glGenBuffers(1, &VertexBufferID1);
	//set the current state to focus on our vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID1);
	//actually memcopy the data - only do this once
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_buffer_data), background_buffer_data, GL_DYNAMIC_DRAW);
	
	//we need to set up the vertex array
	glEnableVertexAttribArray(0);
	//key function to get up how many elements to pull out at a time (3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    
    //*********************************
    
    
    
    //generate the VAO
    glGenVertexArrays(1, &VertexArrayID2);
    glBindVertexArray(VertexArrayID2);
    
    buildRays();
    
    //generate vertex buffer to hand off to OGL
    glGenBuffers(1, &VertexBufferID2);
    //set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID2);
    //actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(sun_buffer_data), sun_buffer_data, GL_DYNAMIC_DRAW);
    
//    buildIndexBuff();
//
//    glGenBuffers(1, &IndexBufferID1);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID1);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    

    //we need to set up the vertex array
    glEnableVertexAttribArray(0);
    //key function to get up how many elements to pull out at a time (3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glBindVertexArray(0);

}

//General OGL initialization - set OGL state here
static void init()
{
	GLSL::checkVersion();

	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize the GLSL program.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RESOURCE_DIR + "simple_vert1.glsl", RESOURCE_DIR + "simple_frag1.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addAttribute("vertPos");
    
    //**********************
    prog2 = make_shared<Program>();
    prog2->setVerbose(true);
    prog2->setShaderNames(RESOURCE_DIR + "simple_vert2.glsl", RESOURCE_DIR + "simple_frag2.glsl");
    prog2->init();
    prog2->addUniform("P");
    prog2->addUniform("MV");
    prog2->addAttribute("vertPos");
    prog2->addUniform("offsetx");

}


/****DRAW
This is the most important function in your program - this is where you
will actually issue the commands to draw any geometry you have set up to
draw
********/
static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width/(float)height;
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create the matrix stacks - please leave these alone for now
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	// Apply orthographic projection.
	P->pushMatrix();
	if (width > height) {
		P->ortho(-1*aspect, 1*aspect, -1, 1, -2, 100.0f);
	} else {
		P->ortho(-1, 1, -1*1/aspect, 1*1/aspect, -2, 100.0f);
	}
	MV->pushMatrix();
    

    // Draw the triangle using GLSL.
    prog2->bind();
    
    //send the matrices to the shaders
    glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    
    static float f = 0.0;
    float col = (sin(f));
    f+=0.01;
    
    glUniform1f(prog2->getUniform("offsetx"),col);


    glBindVertexArray(VertexArrayID2);
    glDrawArrays(GL_TRIANGLES, 0, 2*(TRI*9)*3 + (TRI*9)*3);//actually draw from vertex 0, 3 vertices
    
    prog2->unbind();
    
    // Draw the triangle using GLSL.
    prog->bind();
    
    //send the matrices to the shaders
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    
    glBindVertexArray(VertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 6);//actually draw from vertex 0, 3 vertices
    

    glBindVertexArray(0);
	
	prog->unbind();

	// Pop matrix stacks.
	MV->popMatrix();
	P->popMatrix();
}

int main(int argc, char **argv)
{
//   if(argc < 2) {
//      cout << "Please specify the resource directory." << endl;
//      return 0;
//   }
   RESOURCE_DIR = "../../resources/";

	/* your main will always include a similar set up to establish your window
      and GL context, etc. */

	// Set error callback as openGL will report errors but they need a call back
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	//request the highest possible version of OGL - important for mac
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	pixW = 480;
	pixH = 480;
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(pixW, pixH, "hello triangle", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	//set the mouse call back
//    glfwSetMouseButtonCallback(window, mouse_callback);
	//set the window resize call back
	glfwSetFramebufferSizeCallback(window, resize_callback);

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	init();
	initGeom();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
