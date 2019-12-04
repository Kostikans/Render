#include "Render.h"
#include <QSurfaceFormat>
#include <QVector>
#include <qopenglfunctions_4_4_core.h>



#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024


static float deltaTime = 1.0f / 60.0f;
static bool mouseCapture = true;
static QPoint lastMousePosition;

Render::Render(QWidget* parent)
	: QOpenGLWidget(parent), indexBuffer(QOpenGLBuffer::IndexBuffer)
{
	makeCurrent();
	camera = new Camera(QVector3D(0.0f, 1.0f, 3.0f));


	QWidget::setCursor(Qt::BlankCursor);
	QCursor::setPos(frameGeometry().center() - this->mapFromGlobal(geometry().center()));
	setMouseTracking(true);
	this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
	for (int i = 0; i < 1024; ++i)
		keys[i] = false;
	sphere = new Sphere();
	cube = new Cube();
	lamp = new Lamp();
	floor = new Floor();
	timer.start(6);
	sphere->translate(QVector3D(0.0, 3.0f, 0.0f));
	lamp->translate(QVector3D(-3.0f, 3.0f, 3.0f));
	floor->translate(QVector3D(0.0f, -5.0f, 0.0f));

	QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(increment()));
}

Render::~Render()
{

}

void Render::initializeGL()
{
	functions.initializeOpenGLFunctions();
	QSurfaceFormat format;
	format.setSamples(16);
	format.setDepthBufferSize(24);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setVersion(5, 2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);
	this->setFormat(format);
	

	functions.glGenFramebuffers(1, &depthMapFBO);
	functions.glGenTextures(1, &depthCubemap);
	functions.glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		functions.glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	functions.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	functions.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	functions.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	functions.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	functions.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	functions.glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);


	QOpenGLExtraFunctions func;
	func.initializeOpenGLFunctions();
	func.glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	QOpenGLFunctions_4_4_Core  m_openGL31Functions;
	m_openGL31Functions.initializeOpenGLFunctions();
	m_openGL31Functions.glDrawBuffer(GL_NONE);
	m_openGL31Functions.glReadBuffer(GL_NONE);


	functions.glBindFramebuffer(GL_FRAMEBUFFER, 0);

	functions.glEnable(GL_DEPTH_TEST);
	functions.glEnable(GL_CULL_FACE);
	functions.glEnable(GL_MULTISAMPLE);
	functions.glEnable(GL_TEXTURE_2D);
	initShaders();

	float aspect = static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);
	float m_near = 0.01f;
	float m_far = 100.0f;
	shadowProjMatrix.perspective(90.0f, aspect, m_near, m_far);

	cube->init(1.0f, "E:/download/8640003215_50cc68f8cf_b.jpg");
	sphere->init(1.0f, "E:/download/45614125-bcb2ce00-ba68-11e8-903b-f22addb83ca5.jpg");
	lamp->init(0.4f);
	floor->init(20.0f, 1.0f, 20.0f, "E:/download/8640003215_50cc68f8cf_b.jpg");
	//model = new Model("C:\\Users\\Kostya\\source\\repos\\dodge\\source\\Dodge Challenger\\mesh.obj");
	//model->translate(QVector3D(5.0f, 2.0f, 0.0f));
}

void Render::resizeGL(int w, int h)
{
	float aspect = w / static_cast<float>(h);
	projectionMatrix.setToIdentity();
	projectionMatrix.perspective(50.0f, aspect, 0.01f, 100.0f);
}

void Render::paintGL()
{
	functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	functions.glClearColor(0.0f, 0.1f, 0.1f, 1.0f);

	functions.glActiveTexture(GL_TEXTURE1);
	functions.glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	lamp->setPosition(QVector3D(sinf(kek) * 5.0f, 3.0f, cosf(kek) * 5.0f));

	// отрисовка в буфер теней
	buildShadowMap();
	for (unsigned int i = 0; i < 6; ++i)
	{
		QString name = "shadowMatrices[" + QString::number(i) + "]";
		depthShaderProgram.setUniformValue(name.toStdString().c_str(), shadowTransform[i]);
		depthShaderProgram.setUniformValue("far_plane", 100.0f);
		depthShaderProgram.setUniformValue("lightPos", lamp->getLampPosition());
	}
	cube->draw(&depthShaderProgram,&functions);
	sphere->draw(&depthShaderProgram, &functions);
	floor->draw(&depthShaderProgram, &functions);
	lamp->draw(&depthShaderProgram, &functions);


	depthShaderProgram.release();
	functions.glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// рендер обычной сцены
	functions.glViewport(0, 0, 1600, 900);
	functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	QPoint mousePosition = QCursor::pos();
	if (mousePosition != lastMousePosition && mouseCapture)
	{

		lastMousePosition = frameGeometry().center();
		float xoffset = mousePosition.x() - lastMousePosition.x();
		float yoffset = lastMousePosition.y() - mousePosition.y();
		camera->ProcessMouseMovement(xoffset, yoffset);
		QCursor::setPos(frameGeometry().center());
	}

	shadowShaderProgram.bind();
	functions.glActiveTexture(GL_TEXTURE0);
	shadowShaderProgram.setUniformValue("depthMap", 1);
	shadowShaderProgram.setUniformValue("projectionMatrix", projectionMatrix);
	shadowShaderProgram.setUniformValue("viewMatrix", camera->GetViewMatrix());

	shadowShaderProgram.setUniformValue("lightPos", lamp->getLampPosition());
	shadowShaderProgram.setUniformValue("viewPos", camera->getEyePos());
	shadowShaderProgram.setUniformValue("far_plane", 100.0f);


	cube->draw(&shadowShaderProgram, &functions);
	sphere->draw(&shadowShaderProgram, &functions);
	floor->draw(&shadowShaderProgram, &functions);


	shadowShaderProgram.release();
	lampShaderProgram.bind();

	lampShaderProgram.setUniformValue("projectionMatrix", projectionMatrix);
	lampShaderProgram.setUniformValue("viewMatrix", camera->GetViewMatrix());
	lamp->draw(&lampShaderProgram, &functions);

	shadowTransform.clear();
	pollEvents();
}
void Render::initShaders()
{

	if (!shaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vertexshader.vert"))
		close();
	if (!shaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fragmentshader.frag"))
		close();
	if (!shaderProgram.link())
		close();

	if (!lampShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/lamp.vert"))
		close();
	if (!lampShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/lamp.frag"))
		close();
	if (!lampShaderProgram.link())
		close();

	if (!shadowShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/ShadowMapping.vert"))
		close();
	if (!shadowShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/ShadowMapping.frag"))
		close();
	if (!shadowShaderProgram.link())
		close();

	if (!depthShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/depth.vert"))
		close();
	if (!depthShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/depth.frag"))
		close();
	if (!depthShaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, ":/Shaders/depth.geom"))
		close();
	if (!depthShaderProgram.link())
		close();
}

void Render::keyPressEvent(QKeyEvent* e) noexcept
{
	if (e->key() == Qt::Key::Key_Escape)
		close();

	if (e->nativeVirtualKey() == Qt::Key_W)
		keys[Qt::Key_W] = true;

	if (e->nativeVirtualKey() == Qt::Key_S)
		keys[Qt::Key_S] = true;

	if (e->nativeVirtualKey() == Qt::Key_A)
		keys[Qt::Key_A] = true;

	if (e->nativeVirtualKey() == Qt::Key_D)
		keys[Qt::Key_D] = true;

}

void Render::keyReleaseEvent(QKeyEvent* e) noexcept
{
	if (e->nativeVirtualKey() == Qt::Key_W)
		keys[Qt::Key_W] = false;

	if (e->nativeVirtualKey() == Qt::Key_S)
		keys[Qt::Key_S] = false;

	if (e->nativeVirtualKey() == Qt::Key_A)
		keys[Qt::Key_A] = false;

	if (e->nativeVirtualKey() == Qt::Key_D)
		keys[Qt::Key_D] = false;

}

void Render::pollEvents() noexcept
{
	if (keys[Qt::Key_W] == true)
		camera->ProcessKeyboard(FORWARD, deltaTime);

	if (keys[Qt::Key_S] == true)
		camera->ProcessKeyboard(BACKWARD, deltaTime);

	if (keys[Qt::Key_A] == true)
		camera->ProcessKeyboard(LEFT, deltaTime);

	if (keys[Qt::Key_D] == true)
		camera->ProcessKeyboard(RIGHT, deltaTime);

	update();
}

void Render::buildShadowMap() noexcept
{
	functions.glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	functions.glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	functions.glClear(GL_DEPTH_BUFFER_BIT);
	depthShaderProgram.bind();
	QVector3D lightPos = lamp->getLampPosition();
	QMatrix4x4 tempShadowMatrix;

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(-1.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(0.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

	tempShadowMatrix.setToIdentity();
	tempShadowMatrix.lookAt(lightPos, lightPos + QVector3D(0.0f, 0.0f, -1.0f), QVector3D(0.0f, -1.0f, 0.0f));
	shadowTransform.push_back(shadowProjMatrix * tempShadowMatrix);

}

void Render::increment() noexcept
{
	kek += 0.001f;
}

void Render::focusOutEvent(QFocusEvent* e) noexcept
{
	mouseCapture = false;
}

void Render::focusInEvent(QFocusEvent* e) noexcept
{
	QCursor::setPos(frameGeometry().center());
	mouseCapture = true;
}
