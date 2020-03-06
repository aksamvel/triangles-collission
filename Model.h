#ifndef MODEL_H
#define MODEL_H

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "intersect.h"
#include "Paint.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
struct Triangle {
	glm::vec4 p1;
	glm::vec4 p2;
	glm::vec4 p3;
};

class Model
{
public:
	/*  Model Data */
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// draws the model, and thus all its meshes
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	// Check Triangle collision.
	int checkCollision(Model obj, glm::mat4 transformMatrix1, glm::mat4 transformMatrix2, Camera camera) {

		vector<Triangle> trianglesUnit1 = getTriangles();
		vector<Triangle> trianglesUnit2 = obj.getTriangles();
		int trianglesSizeUnit1 = trianglesUnit1.size();
		int trianglesSizeUnit2 = trianglesUnit2.size();
		for (unsigned int i = 0; i < trianglesSizeUnit1; i++) {
			for (unsigned int f = 0; f < trianglesSizeUnit2; f++) {

				glm::vec4 unit1p1 = transformMatrix1 * trianglesUnit1[i].p1;
				glm::vec4 unit1p2 = transformMatrix1 * trianglesUnit1[i].p2;
				glm::vec4 unit1p3 = transformMatrix1 * trianglesUnit1[i].p3;
				glm::vec4 unit2p1 = transformMatrix2 * trianglesUnit2[f].p1;
				glm::vec4 unit2p2 = transformMatrix2 * trianglesUnit2[f].p2;
				glm::vec4 unit2p3 = transformMatrix2 * trianglesUnit2[f].p3;

				int p;
				double source[3];
				double target[3];
				double p1[3] = { unit1p1.x, unit1p1.y, unit1p1.z };
				double q1[3] = { unit1p2.x, unit1p2.y, unit1p2.z };
				double r1[3] = { unit1p3.x, unit1p3.y, unit1p3.z };
				double p2[3] = { unit2p1.x, unit2p1.y, unit2p1.z };
				double q2[3] = { unit2p2.x, unit2p2.y, unit2p2.z };
				double r2[3] = { unit2p3.x, unit2p3.y, unit2p3.z };

				int result = tri_tri_intersection_test_3d(p1, q1, r1, p2, q2, r2, &p, source, target);
				if (result) {
					std::cout << "Collision found" << endl;
					return 1;
				}
			}
		}
		return 0;
	}

private:
	/*  Functions   */
	vector<Triangle> getTriangles() {
		/*vector<Triangle> triangles = {};

	Triangle triangle;
	triangle.p1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	triangle.p2 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	triangle.p3 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	triangles.push_back(triangle);

	triangle.p1 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	triangle.p2 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	triangle.p3 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	triangles.push_back(triangle);

	triangle.p1 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	triangle.p2 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	triangle.p3 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	triangles.push_back(triangle);

	triangle.p1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	triangle.p2 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	triangle.p3 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	triangles.push_back(triangle);*/

		Triangle triangle;
		
		int verSize = meshes[0].indices.size();
		int trianglesSize = verSize / 3;
		int counter = 0;
		vector<Triangle> triangles;
		for (unsigned int i = 0; i < verSize; i++) {
			switch (counter) {
				case 0:
					triangle.p1.x = meshes[0].vertices[meshes[0].indices[i]].Position.x;
					triangle.p1.y = meshes[0].vertices[meshes[0].indices[i]].Position.y;
					triangle.p1.z = meshes[0].vertices[meshes[0].indices[i]].Position.z;
					triangle.p1.w = meshes[0].vertices[meshes[0].indices[i]].Position.w;
					break;
				case 1:
					triangle.p2.x = meshes[0].vertices[meshes[0].indices[i]].Position.x;
					triangle.p2.y = meshes[0].vertices[meshes[0].indices[i]].Position.y;
					triangle.p2.z = meshes[0].vertices[meshes[0].indices[i]].Position.z;
					triangle.p2.w = meshes[0].vertices[meshes[0].indices[i]].Position.w;
					break;
				case 2:
					triangle.p3.x = meshes[0].vertices[meshes[0].indices[i]].Position.x;
					triangle.p3.y = meshes[0].vertices[meshes[0].indices[i]].Position.y;
					triangle.p3.z = meshes[0].vertices[meshes[0].indices[i]].Position.z;
					triangle.p3.w = meshes[0].vertices[meshes[0].indices[i]].Position.w;
					break;
			}
			
			if (counter >= 2) {
				counter = -1;
				triangles.push_back(triangle);
			}
			counter++;
		}

		return triangles;
	}
	
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec4 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vector.w = 1.0f;
			vertex.Position = vector;
			// normals
			if (mesh->mNormals) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			if (mesh->mTangents) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
			}
			
			// bitangent
			if (mesh->mBitangents) {
				vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			}
			
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
#endif