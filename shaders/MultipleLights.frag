//-----------------------------------------------------------------------------
// Fragment shader for Blinn-Phong lighting with materials
//-----------------------------------------------------------------------------
#version 400 core

out vec4 frag_color;

in GeometryData
{
	float ID;
    vec3 normals;
    vec3 FragPos;
} outGeom;

uniform float ID;

void main()
{ 
    if(ID == outGeom.ID )
    {
        frag_color = vec4(0.4, 0.4, 0.1, 0.40f);
    }
    else
    {
        frag_color = vec4(outGeom.normals, 1.0f);
    }

}
