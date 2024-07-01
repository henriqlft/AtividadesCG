#include "Mesh.h"

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, GLuint textureID, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis)
{
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->position = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;
	this->textureID = textureID;
}

void Mesh::update()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::updatePosition(glm::vec3 position) {
	this->position = position;
}

void Mesh::rotate(float speed, glm::vec3 axis) {
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, (GLfloat)glfwGetTime() / speed, axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::increase(float increaseNumber) {
	float inc = this->scale.x + increaseNumber;

	this->scale = glm::vec3(inc);

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::decrease(float decreaseNumber) {
	float decrease = this->scale.x - decreaseNumber;
	if (this->scale.x > 0.1f) {
		this->scale = glm::vec3(decrease);
	}

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::resetScale(float scale) {
	this->scale = glm::vec3(scale);

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, this->scale);
	shader->setMat4("model", glm::value_ptr(model));
}