#pragma once
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <QOpenGLShaderProgram>
#include <qopenglfunctions.h>

struct Vertex {
	QVector3D Position;
	QVector3D Normal;
	QVector2D TexCoords;
};

struct Texture {
	unsigned int id;
	QString type;
	QString path;
};

class Mesh
{
public:
	QVector<Vertex> vertices;
	QVector<unsigned int> indices;
	QVector<Texture> textures;

	Mesh();
	Mesh(QVector<Vertex>& vertices, QVector<unsigned int>& indices, QVector<Texture>& textures) noexcept;
	void Draw(QOpenGLShaderProgram* shader, QOpenGLFunctions* functions);
private:

	unsigned int VAO, VBO, EBO;

	void setupMesh() noexcept;
};

