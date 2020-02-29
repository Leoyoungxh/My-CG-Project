#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
in  vec3 vFaceIndecies;

out vec4 color;
out vec2 texCoord;
out vec3 normal;
out vec3 l_pos;
out vec3 pos;

uniform mat4 ModelMatrix;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;

void main()
{
    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;
    //normal = vNormal;
    normal = (ModelMatrix * vec4(vNormal, 0)).xyz;
    l_pos = (ModelMatrix * LightPosition).xyz;
    pos = (ModelMatrix * vec4(vPosition,0)).xyz;

 	
    vec4 v1 = ModelMatrix * vec4(vPosition, 1.0);
	vec4 v2 = Projection * ModelView * v1;
	gl_Position = v2;
}
