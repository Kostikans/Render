#pragma once

#include <QtWidgets/QWidget>

#include <QWidget>
#include <QtOpenGL>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QVector4D>
#include "camera.h"
#include <QKeyEvent>
#include <QTimer>
#include "transformable.h"
#include "sphere.h"
#include "cube.h"
#include "lamp.h"
#include "floor.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include "model.h"
#include <QGLFormat>

class Render : public QOpenGLWidget
{
	Q_OBJECT

public:
	Render(QWidget* parent = nullptr);
	Render(const QWidget&) = delete;
	Render& operator =(const QWidget&) = delete;
	~Render();

protected:
	void  initializeGL();
	void  resizeGL(int w, int h);
	void  paintGL();
	void  initShaders();
	void  keyPressEvent(QKeyEvent* e) noexcept;
	void  keyReleaseEvent(QKeyEvent* e) noexcept;
	void  focusOutEvent(QFocusEvent* e) noexcept;
	void  focusInEvent(QFocusEvent* e) noexcept;
	void  pollEvents() noexcept;
	void  buildShadowMap() noexcept;

public slots:
	void  increment() noexcept;

private:
	QMatrix4x4                projectionMatrix;
	QMatrix4x4                shadowProjMatrix;
	QOpenGLShaderProgram      shaderProgram;
	QOpenGLShaderProgram      lampShaderProgram;
	QOpenGLShaderProgram      depthShaderProgram;
	QOpenGLShaderProgram      shadowShaderProgram;
	QOpenGLBuffer             vertexBuffer;
	QOpenGLBuffer             indexBuffer;
	Camera* camera;
	QTimer                    timer;
	float                     kek;
	bool                      keys[1024];
	Sphere* sphere;
	Cube* cube;
	Lamp* lamp;
	Floor* floor;
	QOpenGLFunctions          functions;
	QOpenGLFramebufferObject* depthBuffer;
	QVector<QMatrix4x4>       shadowTransform;
	QOpenGLTexture*           cubeMap;
	unsigned int              depthMapFBO;
	unsigned int              depthCubemap;
	QGLFormat*                form;
	QOpenGLContext            context;
	Model*				      model;
};
