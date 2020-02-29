#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
in  vec3 vFaceIndecies;


out vec4 color;
out vec2 texCoord;
out vec4 normal;

uniform mat4 ModelMatrix;   //模型矩阵
uniform mat4 ModelView;     //模视矩阵
uniform mat4 Projection;    //投影矩阵


void main()
{
    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;
    normal = vec4(vNormal, 0);

 	
    vec4 v1 = ModelMatrix * vec4(vPosition, 1.0);

	vec4 v3 = Projection * ModelView * v1;
	gl_Position = v3;
}
