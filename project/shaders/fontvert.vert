#version 460 core

layout (location = 0) in vec3 verticles;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float id;

out vec2 TexCoord;
out float ID;

void main(){
	TexCoord = texCoord;
	ID = id;
	gl_Position = vec4(vec3(verticles.xyz),1.0);
}
