#version 330 core
out vec3 color;

in vec2 pos;

void main()
{
	color = vec3(0.0, 0.0, 1.0-(0.6*abs(length(pos))));
}
