#include "Cube.h"
#include "QtMath"
#include <QOpenGLFunctions>

#define PI   3.14159265f

Cube::Cube()
	: indexBuffer(QOpenGLBuffer::IndexBuffer), m_scale(1.0f)
{

}

Cube::~Cube()
{
	vertexBuffer.destroy();
	indexBuffer.destroy();
	texture->destroy();
}

void Cube::draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions)
{
	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();
	modelMatrix.translate(m_position);
	modelMatrix.rotate(m_rotate);
	modelMatrix.scale(m_scale);

	shaderProgram->bind();
	shaderProgram->setUniformValue("modelMatrix", modelMatrix);
	shaderProgram->setUniformValue("texture_diffuseN", 0);
	vertexBuffer.bind();
	int offset = 0;

	int vertLoc = shaderProgram->attributeLocation("aVertex");
	shaderProgram->enableAttributeArray(vertLoc);
	shaderProgram->setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

	offset += sizeof(QVector3D);

	int texLoc = shaderProgram->attributeLocation("aTexture");
	shaderProgram->enableAttributeArray(texLoc);
	shaderProgram->setAttributeBuffer(texLoc, GL_FLOAT, offset, 2, sizeof(VertexData));

	offset += sizeof(QVector2D);

	int normLoc = shaderProgram->attributeLocation("aNormal");
	shaderProgram->enableAttributeArray(normLoc);
	shaderProgram->setAttributeBuffer(normLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

	indexBuffer.bind();
	texture->bind();
	functions->glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
	texture->release();
	vertexBuffer.release();
	indexBuffer.release();
}

void Cube::translate(const QVector3D& _translate) noexcept
{
	m_position += _translate;
}

void Cube::rotate(const QQuaternion& _rotate) noexcept
{
	m_rotate *= _rotate;
}

void Cube::scale(const float& _scale) noexcept
{
	m_scale = _scale;
}

void Cube::init(const float& width, const char* filePath)
{
	QVector<VertexData> cubeData;

	cubeData.append(VertexData(QVector3D(-width, width, width), QVector2D(0.0f, width), QVector3D(0.0f, 0.0f, 1.0f)));
	cubeData.append(VertexData(QVector3D(-width, -width, width), QVector2D(0.0f, 0.0), QVector3D(0.0f, 0.0f, 1.0f)));
	cubeData.append(VertexData(QVector3D(width, width, width), QVector2D(width, width), QVector3D(0.0f, 0.0f, 1.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, width), QVector2D(width, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));

	cubeData.append(VertexData(QVector3D(width, width, width), QVector2D(0.0f, width), QVector3D(1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, width), QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, width, -width), QVector2D(width, width), QVector3D(1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, -width), QVector2D(width, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));

	cubeData.append(VertexData(QVector3D(width, width, width), QVector2D(0.0f, width), QVector3D(0.0f, 1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, width, -width), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, width, width), QVector2D(width, width), QVector3D(0.0f, 1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, width, -width), QVector2D(width, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));

	cubeData.append(VertexData(QVector3D(width, width, -width), QVector2D(0.0f, width), QVector3D(0.0f, 0.0f, -1.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, -width), QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));
	cubeData.append(VertexData(QVector3D(-width, width, -width), QVector2D(width, width), QVector3D(0.0f, 0.0f, -1.0f)));
	cubeData.append(VertexData(QVector3D(-width, -width, -width), QVector2D(width, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));

	cubeData.append(VertexData(QVector3D(-width, width, width), QVector2D(0.0f, width), QVector3D(-1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, width, -width), QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, -width, width), QVector2D(width, width), QVector3D(-1.0f, 0.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, -width, -width), QVector2D(width, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));

	cubeData.append(VertexData(QVector3D(-width, -width, width), QVector2D(0.0f, width), QVector3D(0.0f, -1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(-width, -width, -width), QVector2D(0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, width), QVector2D(width, width), QVector3D(0.0f, -1.0f, 0.0f)));
	cubeData.append(VertexData(QVector3D(width, -width, -width), QVector2D(width, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));

	QVector<GLuint> indexes;
	for (int i = 0; i < 24; i += 4)
	{
		indexes.append(i + 0);
		indexes.append(i + 1);
		indexes.append(i + 2);
		indexes.append(i + 2);
		indexes.append(i + 1);
		indexes.append(i + 3);
	}

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.allocate(cubeData.data(), cubeData.size() * sizeof(VertexData));
	vertexBuffer.release();

	indexBuffer.create();
	indexBuffer.bind();
	indexBuffer.allocate(indexes.data(), indexes.size() * sizeof(GLuint));
	indexBuffer.release();

	texture = new QOpenGLTexture((QImage(filePath)).mirrored());
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);
}
