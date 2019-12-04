#include "Mesh.h"
#include <QOpenGLExtraFunctions>

Mesh::Mesh()
{

}

Mesh::Mesh(QVector<Vertex>& vertices, QVector<unsigned int>& indices, QVector<Texture>& textures) noexcept
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;


	setupMesh();
}

void Mesh::Draw(QOpenGLShaderProgram* shader, QOpenGLFunctions* functions)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		functions->glActiveTexture(GL_TEXTURE0 + i);
		QString number;
		QString name = textures[i].type;
		if (name == "texture_diffuse")
			number = QString::number(diffuseNr++);
		else if (name == "texture_specular")
			number = QString::number(specularNr++);
		else if (name == "texture_normal")
			number = QString::number(normalNr++);
		else if (name == "texture_height")
			number = QString::number(heightNr++);


		shader->setUniformValue(name.toStdString().c_str(), i);

		functions->glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	QOpenGLExtraFunctions func;
	func.initializeOpenGLFunctions();
	func.glBindVertexArray(VAO);
	functions->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	func.glBindVertexArray(0);

	functions->glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() noexcept
{
	QOpenGLExtraFunctions func;
	func.initializeOpenGLFunctions();
	func.glGenVertexArrays(1, &VAO);
	func.glGenBuffers(1, &VBO);
	func.glGenBuffers(1, &EBO);

	func.glBindVertexArray(VAO);

	func.glBindBuffer(GL_ARRAY_BUFFER, VBO);

	func.glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	func.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	func.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	func.glEnableVertexAttribArray(0);
	func.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	func.glEnableVertexAttribArray(1);
	func.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

	func.glEnableVertexAttribArray(2);
	func.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));


	func.glBindVertexArray(0);
}