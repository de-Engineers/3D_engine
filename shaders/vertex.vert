#version 460 core

layout (location = 0) in vec3 verticles;
	
void main(){
	gl_Position = vec4(vec3(verticles.xyz),1.0);
}
