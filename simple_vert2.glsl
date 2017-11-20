#version 330 core
layout(location = 0) in vec3 vertPos;
uniform mat4 P;
uniform mat4 MV;
uniform float offsetx;


out vec2 pos;

void main()
{
    vec2 offset = vec2(offsetx, offsetx)/3;
	gl_Position = P * MV * vec4(vertPos, 1.0);
    
    pos = gl_Position.xy;
    
    if(abs(length(gl_Position.xy))==0.4){
        gl_Position.xy+=((3)*(gl_Position.xy*length(offset)));
        pos+=((3)*(pos*length(offset)));
    }
    
    else{
        gl_Position.xy-=((1.25)*(gl_Position.xy*length(offset)));
        pos-=((1.25)*(pos*length(offset)));
    }
}
