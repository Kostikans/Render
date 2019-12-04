#include "VertexData.h"

VertexData::VertexData()
{

}

VertexData::VertexData(const QVector3D& _position, const QVector2D& _texCoord, const QVector3D& _normal)
{
	position = _position;
	texCoord = _texCoord;
	normal = _normal;
}

void VertexData::setPos(const QVector3D& _position)
{
	position = _position;
}

void VertexData::setTexCoord(const QVector2D& _texCoord)
{
	texCoord = _texCoord;
}

void VertexData::setNorm(const QVector3D& _normal)
{
	normal = _normal;
}

