//-----------------------------------------------------------------------------
// Basic Mesh class
//-----------------------------------------------------------------------------
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "glm/glm.hpp"
#include "collision/Collisions.h"


#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct BoxData
{
	glm::vec3 Max;
	glm::vec3 Min;
};

struct GPUdata
{
	COLLIDE::AAB_values BoundingBox;
	std::vector<glm::vec3> Vertices;
	GLuint mVBO, mVAO;
	std::string ObjectName;
	int identifier;
};
struct MapData
{
	std::vector<GPUdata> vertexObject;
	std::vector<BoxData> ParsedData;
};



class Mesh
{
public:

	 Mesh();
	~Mesh();

	void InitMapDataOpenglObject(GPUdata& data);

	bool LoadJSON(const std::string& filename);
	void WriteJson(const std::string& filename);
	void RenderJsonMap();
	void drawBatchedMap();
	
	void DeleteBox(int ID);
	void UpdateVertexBuffer();
	void CheckIntersctions(COLLIDE::Ray Aim,  unsigned int shaderID);

private:

	void initBuffers();

	MapData mapData;
	int ActiveID = 0;

	bool mLoaded;
	std::vector<glm::vec3> mVertices;
	std::vector<float> Identifiers;
	GLuint mVBO, IVBO, mVAO;
	const char* buf;

};
#endif //MESH_H
