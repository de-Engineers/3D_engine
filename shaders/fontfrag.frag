#version 460 core

in vec2 TexCoord;
in float ID;

out vec4 FragColor;

uniform vec2 fov;
uniform sampler2D font;
uniform ivec2 reso;
uniform vec3 color;
uniform float sensitivity;

uniform ivec3 colorSel;
uniform ivec3 metadt1;
uniform ivec3 metadt2;
uniform ivec3 metadt3;
uniform ivec3 metadt4;
uniform ivec3 metadt5;
uniform ivec3 metadt6;

uniform int setting;

uniform float playerFlightSpeed;

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

void slider(){
	if(TexCoord.x<0.003){
		FragColor = vec4(0.5);
	}
	else if(TexCoord.x>0.997){
		FragColor = vec4(0.5);
	}
	else if(TexCoord.y<0.05){
		FragColor = vec4(0.5);
	}
	else if(TexCoord.y>0.95){
		FragColor = vec4(0.5);
	}
	else{
		FragColor = vec4(vec3(TexCoord.x),1.0);
	}
}

void main(){
	switch(int(ID)){
	case 0:
		FragColor = texture(font,vec2(TexCoord.x,1.0 - TexCoord.y));
		FragColor.a = 1.0-FragColor.r;
		FragColor.g *= 5.0;
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
	}
	case 7:{
		FragColor = vec4(color,1.0);
		return;
	}
	case 8:
		slider();
		if(TexCoord.x > float(colorSel.r)/255.0-0.01 && TexCoord.x < float(colorSel.r)/255.0+0.01){
			FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
		}
		return;
	case 9:
		slider();
		if(TexCoord.x > float(colorSel.g)/255.0-0.01 && TexCoord.x < float(colorSel.g)/255.0+0.01){
			FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
		}
		return;
	case 10:
		slider();
		if(TexCoord.x > float(colorSel.b)/255.0-0.01 && TexCoord.x < float(colorSel.b)/255.0+0.01){
			FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
		}
		return;
	case 11:
		slider();
		if((setting / 0x40) % 2==1){
			if(TexCoord.x > float(metadt4.r)/255.0-0.01 && TexCoord.x < float(metadt4.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt1.r)/255.0-0.01 && TexCoord.x < float(metadt1.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		return;
	case 12:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt4.g)/255.0-0.01 && TexCoord.x < float(metadt4.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt1.g)/255.0-0.01 && TexCoord.x < float(metadt1.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		return;
	case 13:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt4.b)/255.0-0.01 && TexCoord.x < float(metadt4.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt1.b)/255.0-0.01 && TexCoord.x < float(metadt1.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		return;
	case 14:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt5.r)/255.0-0.01 && TexCoord.x < float(metadt5.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt2.r)/255.0-0.01 && TexCoord.x < float(metadt2.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		return;
	case 15:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt5.g)/255.0-0.01 && TexCoord.x < float(metadt5.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt2.g)/255.0-0.01 && TexCoord.x < float(metadt2.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		return;
	case 16:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt5.b)/255.0-0.01 && TexCoord.x < float(metadt5.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt2.b)/255.0-0.01 && TexCoord.x < float(metadt2.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		return;
	case 17:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt6.r)/255.0-0.01 && TexCoord.x < float(metadt6.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt3.r)/255.0-0.01 && TexCoord.x < float(metadt3.r)/255.0+0.01){
				FragColor = vec4(vec3(1.0,0.0,0.0),1.0);
			}
		}
		return;
	case 18:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt6.g)/255.0-0.01 && TexCoord.x < float(metadt6.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt3.g)/255.0-0.01 && TexCoord.x < float(metadt3.g)/255.0+0.01){
				FragColor = vec4(vec3(0.0,1.0,0.0),1.0);
			}
		}
		return;
	case 19:
		slider();
		if(setting / 0x40 % 2==1){
			if(TexCoord.x > float(metadt6.b)/255.0-0.01 && TexCoord.x < float(metadt6.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		else{
			if(TexCoord.x > float(metadt3.b)/255.0-0.01 && TexCoord.x < float(metadt3.b)/255.0+0.01){
				FragColor = vec4(vec3(0.0,0.0,1.0),1.0);
			}
		}
		return;
	case 20:
		slider();
		if(TexCoord.x > fov.y-0.01 && TexCoord.x < fov.y+0.01){
			FragColor = vec4(vec3(1.0,1.0,1.0),1.0);
		}
		return;
	case 21:
		slider();
		if(TexCoord.x > sensitivity-0.01 && TexCoord.x < sensitivity+0.01){
			FragColor = vec4(vec3(1.0,1.0,1.0),1.0);
		}
		return;
	}
}
