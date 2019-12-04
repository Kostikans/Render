#pragma once
#include <QVector3D>
#include <QVector2D>
#include <QVector4D>

class VertexData
{
public:
	~VertexData() {}
	VertexData();
	VertexData(const QVector3D& _position, const QVector2D& _texCoord, const QVector3D& _normal);
	void setPos(const QVector3D& _position);
	void setTexCoord(const QVector2D& _texCoord);
	void setNorm(const QVector3D& _normal);
private:
	QVector3D position;
	QVector2D texCoord;
	QVector3D normal;
};

