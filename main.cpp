// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "intersect.h"
#include "Paint.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1900;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle Collision", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();


	// Configure global opengl state.
	glEnable(GL_DEPTH_TEST);

	Shader unit1Shader("model_loading.vs", "model_loading.fs");
	Shader unit2Shader("model_loading.vs", "model_loading.fs");

	// Load models.
	Model unit1("Char/char.obj");
	Model unit2("Char/char.obj");

	int collision = 0;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle input.
		processInput(window);

		// Render part.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use my shader programs.
		unit1Shader.use();
		unit2Shader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		unit1Shader.setMat4("projection", projection);
		unit2Shader.setMat4("projection", projection);
		unit1Shader.setMat4("view", view);
		unit2Shader.setMat4("view", view);


		// TranslationMatrix * RotationMatrix * ScaleMatrix should be exactly in this order.

		// First model.
		glm::mat4 unit1Model = glm::mat4(1.0f);
		// Scale first model.
		 unit1Model = glm::scale(unit1Model, glm::vec3(0.1f, 0.1f, 0.1f));
		// Move first model.
	    unit1Model = glm::translate(unit1Model, glm::vec3(0.0f, 0.0f, -30.00f));
		unit1Model = glm::translate(unit1Model, glm::vec3(0.0f, 0.0f, 1.0f * (GLfloat)glfwGetTime()));
		// Rotate first model.
		//unit1Model = glm::rotate(unit1Model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		unit1Shader.setMat4("model", unit1Model);
		unit1.Draw(unit1Shader);


		// Second model.
		glm::mat4 unit2Model = glm::mat4(1.0f);
		// Scale second model.
		unit2Model = glm::scale(unit2Model, glm::vec3(0.1f, 0.1f, 0.1f));
		// Move to other place.
		unit2Model = glm::translate(unit2Model, glm::vec3(0.0f, 0.0f, 0.0f));
		unit2Model = glm::translate(unit2Model, glm::vec3(0.0f, 0.0f, -1.0f * (GLfloat)glfwGetTime()));
		// Rotate second model.
		//unit2Model = glm::rotate(unit2Model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		unit2Shader.setMat4("model", unit2Model);
		unit2.Draw(unit2Shader);

		// check collision.
		if (!collision) {
			collision = unit2.checkCollision(unit1, unit1Model, unit2Model, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void draw() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}