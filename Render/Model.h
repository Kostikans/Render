#pragma once

#include "transformable.h"
#include <QOpenGLBuffer>
#include "mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


unsigned int TextureFromFile(const char* path, const QString& directory, bool gamma = false);

class Model : public Transformable
{
public:
	Model(const QString& path);
	~Model() override;
	void draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions) override;
	void translate(const QVector3D& _translate) noexcept override;
	void rotate(const QQuaternion& _rotate) noexcept override;
	void scale(const float& _scale) noexcept override;
private:
	void loadModel(const QString& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	QVector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		QString typeName);

	QVector3D m_position;
	QQuaternion m_rotate;
	float m_scale;

	QVector<Texture> textures_loaded;
	QVector<Mesh> meshes;
	QString directory;
};


