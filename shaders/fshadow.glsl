#version 330 core

in vec4 color;
in vec2 texCoord;
in vec4 normal;
in vec3 faceIndecies;

out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;
uniform vec4 shadow;

void main()
{
	// fColor = vec4(1, 0, 0, 0 ) * texture2D( texture, texCoord ); 
    fColor = vec4(0,0,0,1);
    fNormal = normal;
}

