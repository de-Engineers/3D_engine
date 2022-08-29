#version 460 core
out vec4 FragColor;

uniform sampler2D screenTexture;

void main(){ 
    FragColor.a = 1.0;
    FragColor.r = texelFetch(screenTexture,ivec2(gl_FragCoord.xy),0).r;
}