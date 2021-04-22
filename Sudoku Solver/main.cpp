#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>
#include <vector>

#include "ResourceManager.h"
#include "SudokuSolver.h"
#include "TextRenderer.h"

// callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// utility functions
void Init();
void processInput(GLFWwindow* window);

// window settings
const unsigned int SCR_WIDTH = 700;
const unsigned int SCR_HEIGHT = 900;

// time variables
double deltaTime = 0.0f;		// time between current frame and last frame
double lastFrame = 0.0f;		// time of last frame

// mouse variables
double MouseX, MouseY;

// table
int SquareSize = 70;
int TableUpX = (SCR_WIDTH - 9 * SquareSize) / 2;
int TableUpY = 130;

void DrawTable();
bool InTable();

// Sudoku Solver
SudokuSolver* Sudoku;

// Text Renderer
TextRenderer* RenderText;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, false);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cellular Automata", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// configure global OpenGL state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// initialize application
	Init();


	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// time
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// input
		processInput(window);


		// rendering commands here
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawTable();


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete Sudoku;
	delete RenderText;

	// glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Callback functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// todo : make a button
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		if (Sudoku->Solve())
			std::cout << "Solve Sudoku\n";
		else
			std::cout << "Impossible Sudoku\n";

	if (InTable())
	{
		int row = ((int)MouseY - TableUpY) / SquareSize + 1;
		int column = ((int)MouseX - TableUpX) / SquareSize + 1;

		std::cout << row << ' ' << column << '\n';

		for (int i = 1; i < 10; i++)
			if (key == GLFW_KEY_0 + i && action == GLFW_PRESS)
				Sudoku->SetTableValue(column, row, i);

		for (int i = 1; i < 10; i++)
			if (key == GLFW_KEY_KP_0 + i && action == GLFW_PRESS)
				Sudoku->SetTableValue(column, row, i);

		if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
			Sudoku->SetTableValue(column, row, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Utility functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint SquareVAO, LineVAO;
void Init()
{
	// SQUARE
	float SquareVertices[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	GLuint SquareVBO;
	glGenVertexArrays(1, &SquareVAO);
	glGenBuffers(1, &SquareVBO);

	glBindVertexArray(SquareVAO);

	glBindBuffer(GL_ARRAY_BUFFER, SquareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SquareVertices), SquareVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// LINE
	float LineVertices[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
	};

	GLuint LineVBO;
	glGenVertexArrays(1, &LineVAO);
	glGenBuffers(1, &LineVBO);

	glBindVertexArray(LineVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertices), LineVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &LineVBO);
	glDeleteBuffers(1, &SquareVBO);

	// load shaders
	ResourceManager::LoadShader("shaders/line.vert", "shaders/line.frag", nullptr, "line");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f);

	ResourceManager::GetShader("line").Use();
	ResourceManager::GetShader("line").SetMatrix4f("projection", projection);

	// configure sudoku solver
	Sudoku = new SudokuSolver();

	// configure text renderer
	RenderText = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
	RenderText->Load("fonts/Antonio-Bold.ttf", 60);
}

void processInput(GLFWwindow* window)
{
	glfwGetCursorPos(window, &MouseX, &MouseY);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														Table
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool InTable()
{
	return TableUpX <= MouseX && MouseX <= TableUpX + 9 * SquareSize && TableUpY <= MouseY && MouseY <= TableUpY + 9 * SquareSize;
}

void DrawTable()
{
	// draw rows and columns
	glm::mat4 model = glm::mat4(1.0f);

	ResourceManager::GetShader("line").Use();
	ResourceManager::GetShader("line").SetVector3f("color", glm::vec3(0.9f, 0.9f, 0.9f));

	glBindVertexArray(LineVAO);
	
	// draw rows
	for (int y = (int)TableUpY, index = 0; y <= TableUpY + 9 * SquareSize; y += SquareSize, index++)
	{
		if (index % 3 == 0)
			glLineWidth(4.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(TableUpX, y, 0.0f));
		model = glm::scale(model, glm::vec3(9.0f * SquareSize, 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);

		if (index % 3 == 0)
			glLineWidth(1.0f);
	}

	// draw columns
	for (int x = (int)TableUpX, index = 0; x <= TableUpX + 9 * SquareSize; x += SquareSize, index++)
	{
		if (index % 3 == 0)
			glLineWidth(4.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, TableUpY, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(9.0f * SquareSize, 0.0f, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);

		glDrawArrays(GL_LINES, 0, 2);

		if (index % 3 == 0)
			glLineWidth(1.0f);
	}

	// draw selected box
	if (InTable())
	{
		int row = ((int)MouseY - TableUpY) / SquareSize;
		int column = ((int)MouseX - TableUpX) / SquareSize;

		glBindVertexArray(SquareVAO);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f * TableUpX + column * SquareSize, 1.0f * TableUpY + row * SquareSize, 0.0f));
		model = glm::scale(model, glm::vec3((float)SquareSize, (float)SquareSize, 0.0f));
		ResourceManager::GetShader("line").SetMatrix4f("model", model);
		ResourceManager::GetShader("line").SetVector3f("color", glm::vec3(0.6, 0.6, 0.6));

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// draw numbers
	for (int x = 1; x < 10; x++)
	{
		for (int y = 1; y < 10; y++)
		{
			int value = Sudoku->GetTableValue(x, y);
			if (value != 0)
				RenderText->RenderText(std::to_string(value), 1.0f * TableUpX + (x - 1) * SquareSize + 20.0f, 1.0f * TableUpY + (y - 1) * SquareSize + 10.0f, 1.0f);
		}
	}
}

