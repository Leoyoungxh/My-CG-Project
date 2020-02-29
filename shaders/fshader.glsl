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
    fColor = texture2D( texture, texCoord );
    fNormal = normal;
}

