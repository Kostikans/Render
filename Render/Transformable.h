#pragma once

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include "vertexdata.h"


class Transformable
{
public:
	Transformable() = default;
	virtual ~Transformable() = default;
	virtual void draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions) = 0;
	virtual void translate(const QVector3D& _translate) = 0;
	virtual void rotate(const QQuaternion& _rotate) = 0;
	virtual void scale(const float& _scale) = 0;
};
