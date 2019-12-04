#pragma once
#include "transformable.h"
#include <QOpenGLBuffer>


class Sphere : public Transformable
{
public:
	Sphere();
	~Sphere() override;
	void draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions) override;
	void translate(const QVector3D& _translate) noexcept override;
	void rotate(const QQuaternion& _rotate) noexcept override;
	void scale(const float& _scale) noexcept override;
	void init(const float& radius, const char* filePath);

private:
	QOpenGLBuffer vertexBuffer;
	QOpenGLBuffer indexBuffer;
	QVector3D m_position;
	QQuaternion m_rotate;
	float m_scale;

	QOpenGLTexture* texture;
};