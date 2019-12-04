#include "Sphere.h"
#include "QtMath"

#define PI   3.14159265f

Sphere::Sphere()
	: indexBuffer(QOpenGLBuffer::IndexBuffer), m_scale(1.0f)
{

}

Sphere::~Sphere()
{
	vertexBuffer.destroy();
	indexBuffer.destroy();
	texture->destroy();
}

void Sphere::draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions)
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

void Sphere::translate(const QVector3D& _translate) noexcept
{
	m_position += _translate;
}

void Sphere::rotate(const QQuaternion& _rotate) noexcept
{
	m_rotate *= _rotate;
}

void Sphere::scale(const float& _scale) noexcept
{
	m_scale = _scale;
}

void Sphere::init(const float& radius, const char* filePath)
{
	QVector<VertexData> vertexes;
	float sectorCount = 40;
	float stackCount = 40;
	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / radius;
	float s, t;

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			s = static_cast<float>(j) / sectorCount;
			t = static_cast<float>(i) / stackCount;

			vertexes.push_back(VertexData(QVector3D(x, y, z), QVector2D(s, t), QVector3D(nx, ny, nz)));
		}
	}
	QVector<GLuint> indices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = static_cast<int>(i * (sectorCount + 1));
		k2 = static_cast<int>(k1 + sectorCount + 1);

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != static_cast<int>(stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.allocate(vertexes.data(), vertexes.size() * sizeof(VertexData));
	vertexBuffer.release();

	indexBuffer.create();
	indexBuffer.bind();
	indexBuffer.allocate(indices.data(), indices.size() * sizeof(GLuint));
	indexBuffer.release();

	texture = new QOpenGLTexture((QImage(filePath)).mirrored());
	texture->setMinificationFilter(QOpenGLTexture::Nearest);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);
}
