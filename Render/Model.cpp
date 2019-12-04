#include "Model.h"
#include <QVector>
#include <QDebug>
#include "vertexdata.h"
#include <QOpenGLExtraFunctions>

Model::Model(const QString& path)
	: m_scale(1.0f)
{
	loadModel(path);
}

Model::~Model()
{

}


void Model::draw(QOpenGLShaderProgram* shaderProgram, QOpenGLFunctions* functions)
{
	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();
	modelMatrix.translate(m_position);
	modelMatrix.rotate(m_rotate);
	modelMatrix.scale(m_scale);

	shaderProgram->bind();
	shaderProgram->setUniformValue("modelMatrix", modelMatrix);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shaderProgram,functions);
}

void Model::translate(const QVector3D& _translate) noexcept
{
	m_position += _translate;
}

void Model::rotate(const QQuaternion& _rotate) noexcept
{
	m_rotate *= _rotate;
}

void Model::scale(const float& _scale) noexcept
{
	m_scale = _scale;
}

void Model::loadModel(const QString& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		qDebug() << "ERROR::ASSIMP:: " << importer.GetErrorString();
		return;
	}

	directory = "C:";

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	QVector<Vertex> vertices;
	QVector<unsigned int> indices;
	QVector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		QVector3D vector;

		vector.setX(mesh->mVertices[i].x);
		vector.setY(mesh->mVertices[i].y);
		vector.setZ(mesh->mVertices[i].z);
		vertex.Position = vector;

		vector.setX(mesh->mNormals[i].x);
		vector.setX(mesh->mNormals[i].y);
		vector.setX(mesh->mNormals[i].z);
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0])
		{
			QVector2D vec;
			vec.setX(mesh->mTextureCoords[0][i].x);
			vec.setY(mesh->mTextureCoords[0][i].y);
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = QVector2D(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	QVector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.reserve(textures.size() + diffuseMaps.size());
	for (int i = 0; i < diffuseMaps.size(); ++i)
		textures.push_back(diffuseMaps[i]);

	QVector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.reserve(textures.size() + specularMaps.size());
	for (int i = 0; i < diffuseMaps.size(); ++i)
		textures.push_back(specularMaps[i]);

	QVector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.reserve(textures.size() + normalMaps.size());
	for (int i = 0; i < diffuseMaps.size(); ++i)
		textures.push_back(normalMaps[i]);

	QVector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.reserve(textures.size() + heightMaps.size());
	for (int i = 0; i < diffuseMaps.size(); ++i)
		textures.push_back(heightMaps[i]);


	return Mesh(vertices, indices, textures);
}

QVector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName)
{
	QVector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.toStdString().c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const QString& directory, bool gamma)
{
	QOpenGLExtraFunctions func;
	func.initializeOpenGLFunctions();

	QString filename = QString(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	func.glGenTextures(1, &textureID);

	int width = 0, height = 0, nrComponents = 0;

	QImage temp(path);
	unsigned char* data = temp.bits();
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		func.glBindTexture(GL_TEXTURE_2D, textureID);
		func.glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		func.glGenerateMipmap(GL_TEXTURE_2D);

		func.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		func.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		func.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		func.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else
	{
		qDebug() << "Texture failed to load at path: ";
		temp.detach();
	}

	return textureID;
}
