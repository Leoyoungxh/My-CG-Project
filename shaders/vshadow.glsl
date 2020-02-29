#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
in  vec3 vFaceIndecies;


out vec4 color;
out vec2 texCoord;
out vec4 normal;

uniform mat4 ModelMatrix;
uniform mat4 ModelView;
uniform mat4 Projection;


void main()
{
    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;
    normal = vec4(vNormal, 0);

 	
    vec4 v1 = ModelMatrix * vec4(vPosition, 1.0);
    
	vec4 v2 = vec4(v1.xyz / v1.w,1.0);  // 透视除法
   
	vec4 v3 = Projection * ModelView * v2;
	
	gl_Position = v3;
}
