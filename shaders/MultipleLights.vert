//-----------------------------------------------------------------------------
// Vertex shader for multiple lights
//-----------------------------------------------------------------------------
#version 330 core

layout (location = 0) in vec3 pos;			
layout (location = 1) in float identifier;			

uniform mat4 model;			// model matrix
uniform mat4 view;			// view matrix
uniform mat4 projection;	// projection matrix

out VertexData
{
	float ID;
	vec3 fragPos;
}outData;



void main()
{
    outData.ID = identifier;
    outData.fragPos = vec3(model * vec4(pos, 1.0f));			// vertex position in world space
	gl_Position = projection * view * model * vec4(pos, 1.0f);
}
