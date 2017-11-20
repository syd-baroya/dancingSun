#version 330 core
out vec3 color;
in vec2 pos;

void main()
{
    color = vec3(abs(length(pos)) + 0.4, abs(length(pos))+0.2, .7);

    if(abs(length(pos))<=.4)
        color = vec3(1.0, 1.0-(0.3*abs(length(pos))), 0.7);
}
