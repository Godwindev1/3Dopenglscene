//-----------------------------------------------------------------------------
// Basic Mesh class
//-----------------------------------------------------------------------------
#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Vendor/json.hpp"
#include "glm/gtc/matrix_transform.hpp"

//-----------------------------------------------------------------------------
// split
//
// Params:  s - string to split
//		    t - string to split (ie. delimiter)
//
//Result:  Splits string according to some substring and returns it as a vector.
//-----------------------------------------------------------------------------
std::vector<std::string> split(std::string s, std::string t)
{
	std::vector<std::string> res;
	while(1)
	{
		int pos = s.find(t);
		if(pos == -1)
		{
			res.push_back(s); 
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos+1, s.size() - pos - 1);
	}
	return res;
}


#include <vector>
#include <glm/glm.hpp>

std::vector<float> GetIdentifiers(float identifier)
{
	std::vector<float> IDS =
	{
		identifier, identifier, identifier, identifier, identifier, identifier,
		identifier, identifier, identifier, identifier, identifier, identifier,
		identifier, identifier, identifier, identifier, identifier, identifier,
		identifier, identifier, identifier, identifier, identifier, identifier,
		identifier, identifier, identifier, identifier, identifier, identifier,
		identifier, identifier, identifier, identifier, identifier, identifier
	};

	return IDS;
}

std::vector<glm::vec3> GenerateBoundingBoxVertices(const glm::vec3& Min, const glm::vec3& Max, COLLIDE::AAB_values& BO) {
	glm::vec3 v0(Min.x, Min.y, Min.z); // Bottom-left-front
	glm::vec3 v1(Max.x, Min.y, Min.z); // Bottom-right-front
	glm::vec3 v2(Max.x, Max.y, Min.z); // Top-right-front
	glm::vec3 v3(Min.x, Max.y, Min.z); // Top-left-front

	glm::vec3 v4(Min.x, Min.y, Max.z); // Bottom-left-back
	glm::vec3 v5(Max.x, Min.y, Max.z); // Bottom-right-back
	glm::vec3 v6(Max.x, Max.y, Max.z); // Top-right-back
	glm::vec3 v7(Min.x, Max.y, Max.z); // Top-left-back

	glm::mat4 mode = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
	//set Bounding Data
	BO.Bottom_left_back =  mode * glm::vec4(v4, 1.0f);
	BO.Bottom_left_front=  mode * glm::vec4(v0, 1.0f);
	BO.Bottom_Right_back=  mode * glm::vec4(v5, 1.0f);
	BO.Bottom_Right_front= mode * glm::vec4(v1, 1.0f);
	BO.top_left_back=	   mode * glm::vec4(v7, 1.0f);
	BO.top_left_front=     mode * glm::vec4(v3, 1.0f);
	BO.top_Right_back =    mode * glm::vec4(v6, 1.0f);
	BO.top_Right_front =   mode * glm::vec4(v2, 1.0f);

	BO.minPoint = mode * glm::vec4(Min, 1.0f);
	BO.maxPoint = mode * glm::vec4(Max, 1.0f);

	std::vector<glm::vec3> vertices = 
	{
		// Front face
		v0, v1, v2, v2, v3, v0,
		// Back face
		v4, v5, v6, v6, v7, v4,
		// Left face
		v0, v3, v7, v7, v4, v0,
		// Right face
		v1, v2, v6, v6, v5, v1,
		// Top face
		v3, v2, v6, v6, v7, v3,
		// Bottom face
		v0, v1, v5, v5, v4, v0
	};


	return vertices;
}



//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Mesh::Mesh()
	:mLoaded(false)
{
	buf = "";
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

void Mesh::InitMapDataOpenglObject(GPUdata& OpenglObjects)
{
	int Err = 0;
	Err = glGetError();

	glGenVertexArrays(1, &OpenglObjects.mVAO);
	Err = glGetError();

	glGenBuffers(1, &OpenglObjects.mVBO);
	Err = glGetError();

	glBindVertexArray(OpenglObjects.mVAO);	
	Err = glGetError();

	glBindBuffer(GL_ARRAY_BUFFER, OpenglObjects.mVBO); 
	Err = glGetError();

	glBufferData(GL_ARRAY_BUFFER, OpenglObjects.Vertices.size() * sizeof(glm::vec3), &OpenglObjects.Vertices[0], GL_STATIC_DRAW);
	Err = glGetError();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	Err = glGetError();
	//std::cout << Err << std::endl;

	mapData.vertexObject.push_back(OpenglObjects);
}

//LOADS CUSTOM JSON MAP 
bool Mesh::LoadJSON(const std::string& filename)
{
	std::ifstream fin(filename, std::ios::in);

	std::string Json;
	std::string lineBuffer;
	while (std::getline(fin, lineBuffer))
	{
		Json += lineBuffer;
	}


	// Parse JSON
	nlohmann::json jsonData = nlohmann::json::parse(Json);
	int Identifier = 1;

	for (const auto& item : jsonData) {
		BoxData boxData;
		boxData.Max = glm::vec3(item["Max"]["x"], item["Max"]["z"], item["Max"]["y"]);
		boxData.Min = glm::vec3(item["Min"]["x"], item["Min"]["z"], item["Min"]["y"]);

		mapData.ParsedData.push_back(boxData);
		GPUdata OpenglObjects;
		OpenglObjects.identifier = Identifier;
		OpenglObjects.ObjectName = item["ObjectName"];
		OpenglObjects.Vertices = GenerateBoundingBoxVertices(boxData.Min, boxData.Max, OpenglObjects.BoundingBox);

		mapData.vertexObject.push_back(OpenglObjects);
		mVertices.insert(mVertices.end(), OpenglObjects.Vertices.begin(), OpenglObjects.Vertices.end());
		
		auto IDS = GetIdentifiers(Identifier);
		Identifiers.insert(Identifiers.end(), IDS.begin(), IDS.end());

		Identifier++;
	}

	initBuffers();


	return false;
}

void Mesh::WriteJson(const std::string& filename)
{

	nlohmann::json jsonArray;

	int i = 0;
	for (const auto& data : mapData.ParsedData) {
		nlohmann::json item = {
			{"Max", {{"x", data.Max.x}, {"y", data.Max.z}, {"z", data.Max.y}}},
			{"Min", {{"x", data.Min.x}, {"y", data.Min.z}, {"z", data.Min.y}}},
			{"ObjectName", mapData.vertexObject[i].ObjectName}
		};

		jsonArray.push_back(item);
		i++;
	}

	std::ofstream outFile(filename);
	outFile << jsonArray.dump(4);


	return ;
}

void Mesh::DeleteBox(int ID)
{
	mapData.ParsedData.erase(mapData.ParsedData.begin() + ID);
	mapData.vertexObject.erase(mapData.vertexObject.begin() + ID);


	float singleObjectSize = (36 );
	float offset = (ActiveID) * singleObjectSize;
	mVertices.erase(mVertices.begin() + offset, mVertices.begin() + (offset + singleObjectSize));
	Identifiers.erase(Identifiers.begin() + offset, Identifiers.begin() + (offset + singleObjectSize));

	for (int index = ActiveID; index < mapData.vertexObject.size(); index += 1)
	{
		mapData.vertexObject[index].identifier--;

		int size = (index * 36) + 36;
		for (int j = index * 36; j < size; j += 1)
		{
			Identifiers[j] = mapData.vertexObject[index].identifier;
		}
	}

	UpdateVertexBuffer();
}

void Mesh::UpdateVertexBuffer()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &IVBO);
	
	initBuffers();

	//glm::vec3 f[singleObjectSize] = {};
	//
	//float singleObjectSize = (singleObjectSize * sizeof(glm::vec3));
	//float offset = (ActiveID - 1) * singleObjectSize;
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	//glBufferSubData(GL_ARRAY_BUFFER, offset, singleObjectSize, f);
}

void Mesh::CheckIntersctions(COLLIDE::Ray Aim, unsigned int shaderID)
{
	float Distance = INFINITY;

	for (auto& value : mapData.vertexObject)
	{
		//requires AABS to be Scaled Correctly
		glm::vec3 intersectionPoint;
		if (COLLIDE::intersectRayAABB(Aim, value.BoundingBox, intersectionPoint, false))
		{
			if(glm::distance(Aim.origin, intersectionPoint) <= Distance)
			{
				Distance = glm::distance(Aim.origin, intersectionPoint);

				int loca = glGetUniformLocation(shaderID, "ID");
				glUniform1f(loca, (float)value.identifier);
				ActiveID = value.identifier - 1;
			}
		}
	}
}


void Mesh::RenderJsonMap()
{
	for (auto Box : mapData.vertexObject)
	{
		glBindVertexArray(Box.mVAO);

		glDrawArrays(GL_TRIANGLES, 0, Box.Vertices.size());
		glBindVertexArray(0);
	}
}



//-----------------------------------------------------------------------------
// Create and initialize the vertex buffer and vertex array object
// Must have valid, non-empty std::vector of Vertex objects.
//-----------------------------------------------------------------------------
void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &IVBO);

	glBindVertexArray(mVAO);


	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//identifiers
	glBindBuffer(GL_ARRAY_BUFFER, IVBO);
	glBufferData(GL_ARRAY_BUFFER, Identifiers.size() * sizeof(float), &Identifiers[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(1);


	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Render the mesh
//-----------------------------------------------------------------------------
char ImguiBuffer[200] = {' '};
void Mesh::drawBatchedMap()
{
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	glBindVertexArray(0);

	ImGui::Begin("ObjectInformation");

	ImGui::Text(std::string("ObjectName: " + mapData.vertexObject[ActiveID].ObjectName).c_str());

	if (ImGui::InputText("ObjectName", ImguiBuffer, 200))
	{
		mapData.vertexObject[ActiveID].ObjectName = ImguiBuffer;
		
		for (int i = 0; i < 200; i++)
		{
			ImguiBuffer[i] = NULL;
		}
	}

	if (ImGui::Button("DeleteBox"))
	{
		DeleteBox(ActiveID);
	}


	if (ImGui::Button("Save"))
	{
		WriteJson("saved/SavedMap.json");
	}

	ImGui::End();
}


