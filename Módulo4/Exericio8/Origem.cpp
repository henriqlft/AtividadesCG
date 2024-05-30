/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
 //GLAD
#include <glad/glad.h>

//GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


//Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupGeometry();
int loadTexture(string path);
void loadOBJ(string path);
void loadMTL(string path);

const GLuint WIDTH = 800, HEIGHT = 600;
bool rotateX = false, rotateY = false, rotateZ = false;
vector<GLfloat> positions;
vector<GLfloat> textureCoords;
vector<GLfloat> normals;
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;
string mtlFile = "";
string texturePath = "";
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);



// Função MAIN
int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Atividade Modulo 4 - Adicionando Iluminacao - Henrique Lessinger Feiten", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../shaders/hello.vs", "../shaders/hello.fs");
	loadOBJ("C:/Users/I521889/Documents/GitHub/AtividadesCG/3D_Models/Suzanne/SuzanneTriTextured.obj");
	loadMTL("C:/Users/I521889/Documents/GitHub/AtividadesCG/3D_Models/Suzanne/SuzanneTriTextured.mtl");
	GLuint textureID = loadTexture("C:/Users/I521889/Documents/GitHub/AtividadesCG/3D_Models/Suzanne/Suzanne.png");
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "texBuffer"), 0);

	glm::mat4 projection = glm::mat4(1);
	projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1000.0, 1000.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.5);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -0.6f, 50.0f, 1.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glm::mat4 model = glm::mat4(1);
		float angle = (GLfloat)glfwGetTime();

		model = glm::translate(model, glm::vec3(400.0, 300.0, 100));
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		model = glm::scale(model, glm::vec3(200.0, 200.0, 200.0));
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));

		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	GLuint VAO, VBO[3];

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	return VAO;

}

void loadOBJ(string path)
{
	vector<glm::vec3> vertexIndices;
	vector<glm::vec2> textureIndices;
	vector<glm::vec3> normalIndices;

	ifstream file(path);
	if (!file.is_open())
	{
		cerr << "Erro ao abrir arquivo: " << path << endl;
		return;
	}

	string line;
	while (getline(file, line))
	{
		istringstream iss(line);
		string prefix;
		iss >> prefix;

		if (prefix == "mtllib")
		{
			iss >> mtlFile;
		}
		else if (prefix == "v")
		{
			float x, y, z;
			iss >> x >> y >> z;
			vertexIndices.push_back(glm::vec3(x, y, z));
		}
		else if (prefix == "vt")
		{
			float u, v;
			iss >> u >> v;
			textureIndices.push_back(glm::vec2(u, v));
		}
		else if (prefix == "vn")
		{
			float x, y, z;
			iss >> x >> y >> z;
			normalIndices.push_back(glm::vec3(x, y, z));
		}
		else if (prefix == "f")
		{
			string v1, v2, v3;
			iss >> v1 >> v2 >> v3;

			glm::ivec3 vIndices, tIndices, nIndices;
			istringstream(v1.substr(0, v1.find('/'))) >> vIndices.x;
			istringstream(v1.substr(v1.find('/') + 1, v1.rfind('/') - v1.find('/') - 1)) >> tIndices.x;
			istringstream(v1.substr(v1.rfind('/') + 1)) >> nIndices.x;
			istringstream(v2.substr(0, v2.find('/'))) >> vIndices.y;
			istringstream(v2.substr(v2.find('/') + 1, v2.rfind('/') - v2.find('/') - 1)) >> tIndices.y;
			istringstream(v2.substr(v2.rfind('/') + 1)) >> nIndices.y;
			istringstream(v3.substr(0, v3.find('/'))) >> vIndices.z;
			istringstream(v3.substr(v3.find('/') + 1, v3.rfind('/') - v3.find('/') - 1)) >> tIndices.z;
			istringstream(v3.substr(v3.rfind('/') + 1)) >> nIndices.z;

			for (int i = 0; i < 3; i++)
			{
				const glm::vec3& vertex = vertexIndices[vIndices[i] - 1];
				const glm::vec2& texture = textureIndices[tIndices[i] - 1];
				const glm::vec3& normal = normalIndices[nIndices[i] - 1];

				positions.push_back(vertex.x);
				positions.push_back(vertex.y);
				positions.push_back(vertex.z);
				textureCoords.push_back(texture.x);
				textureCoords.push_back(texture.y);
				normals.push_back(normal.x);
				normals.push_back(normal.y);
				normals.push_back(normal.z);
			}
		}
	}

	file.close();
}


void loadMTL(string path)
{
	string line, readValue;
	ifstream mtlFile(path);

	while (!mtlFile.eof())
	{
		getline(mtlFile, line);

		istringstream iss(line);

		if (line.find("map_Kd") == 0)
		{
			iss >> readValue >> texturePath;
		}
		else if (line.find("Ka") == 0)
		{
			float ka1, ka2, ka3;
			iss >> readValue >> ka1 >> ka2 >> ka3;
			ka.push_back(ka1);
			ka.push_back(ka2);
			ka.push_back(ka3);
		}
		else if (line.find("Ks") == 0)
		{
			float ks1, ks2, ks3;
			iss >> readValue >> ks1 >> ks2 >> ks3;
			ks.push_back(ks1);
			ks.push_back(ks2);
			ks.push_back(ks3);
		}
		else if (line.find("Ns") == 0)
		{
			iss >> readValue >> ns;
		}
	}
	mtlFile.close();
}

int loadTexture(string path)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Erro ao ler arquivo de textura" << endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}