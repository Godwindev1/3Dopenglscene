#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Geometry Shader
in VertexData
{
	float ID;
	vec3 fragPos;
} inData[];


out GeometryData
{
	float ID;
    vec3 normals;
    vec3 FragPos;
} outGeom;


vec3 createNormal(vec3 v1, vec3 v2, vec3 v3)
{
    vec3 edge1 = v2 - v1;
    vec3 edge2 = v3 - v1;
    vec3 normal = normalize(cross(edge1, edge2));
			
    return normal;
}

void main()
{
    outGeom.normals = createNormal( gl_in[0].gl_Position.xyz,  gl_in[1].gl_Position.xyz,  gl_in[2].gl_Position.xyz);

    gl_Position = gl_in[0].gl_Position;
    outGeom.FragPos = inData[0].fragPos;
    outGeom.ID = inData[0].ID;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    outGeom.FragPos = inData[1].fragPos;
        outGeom.ID = inData[1].ID;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    outGeom.FragPos = inData[2].fragPos;
        outGeom.ID = inData[2].ID;
    EmitVertex();
    EndPrimitive();
}
