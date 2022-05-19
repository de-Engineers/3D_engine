#version 460 core

in vec2 TexCoord;
in float ID;

out vec4 FragColor;

uniform sampler2D font;
uniform ivec2 reso;
uniform vec3 color;

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

void main(){
	switch(int(ID)){
	case 0:
		FragColor = texture(font,vec2(TexCoord.x,1.0 - TexCoord.y));
		FragColor.a = 1.0-FragColor.r;
		FragColor.r *= 10.0;
		if(FragColor.r == 0){
			discard;
		}
		return;
	case 1:
		float d = 1.0-pow(distance(vec2(0.5),TexCoord),2.0)*5.0;
		if(d > 0.0){
			FragColor.r = d;
			FragColor.a = 1.0;
			return;
		}
		discard;
	case 2:{
	    float d = sdBox(TexCoord-0.5,vec2(0.45,0.45));
	    if(d<0.02){
	        FragColor = vec4(0.5,0.5,0.5,0.4);
	        return;
	    }
	    if(d<0.05){
	        FragColor = vec4(0.7,0.7,0.7,1.0) * (1.0-abs(-d*30.0+1.0));
			return;
	    }
		return;
	}
	case 3:{
		float dst = distance(vec2(0.5),TexCoord);
		if(dst<0.5){
			FragColor.rgb = vec3(0.8,0.2,0.2) * (1.0-dst);
			FragColor.a = 1.0;
			return;
		}
		discard;
	}
	case 4:{
		float dst = distance(vec2(0.5),TexCoord);
		if(dst<0.5){
			FragColor.rgb = vec3(0.2,0.8,0.2) * (1.0-dst);
			FragColor.a = 1.0;
			return;
		}
		discard;
	}
	case 5:
		discard;
		FragColor = vec4(0.4,0.7,0.5,1.0) * distance(TexCoord,vec2(0.5));
		break;
	case 6:{
		float d = distance(TexCoord,vec2(0.5));
		if(d<0.5){
			FragColor = vec4(1.0,1.0,1.0,1.0) * (1.0-d);
			return;
		}
		discard;
	case 7:{
		FragColor = vec4(color,1.0);
		return;
	}
	}
	}
}
