#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include <iostream>

class Paint
{
	
public:
	static void drawLine(glm::vec4 point1, glm::vec4 point2, Camera camera) {
		// settings
		const unsigned int SCR_WIDTH = 1900;
		const unsigned int SCR_HEIGHT = 1200;
		// camera
		// Build and compile our shader program
		Shader shader("model_loading.vs", "line2.fs");
		shader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// Set up vertex data (and buffer(s)) and attribute pointers
		GLfloat vertices[] = {
			point1.x, point1.y, point1.z, // Left  
			point2.x, point2.y, point2.z, // Left  
		};

		GLuint VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(1.0f, 1.0f, 3.0f));
		shader.setMat4("model", model);
		
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);
	}
};

