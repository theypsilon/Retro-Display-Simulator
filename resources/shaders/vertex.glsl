#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aOffset;

out vec3 FragPos;
out vec3 Normal;
out vec4 ObjectColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec2 gap;
uniform float wave;

void main()
{
	float radius = length(aOffset);
	ObjectColor = vec4(sin(wave * radius * 1) + 0.5, sin(wave * radius * 2) + 0.5, sin(wave * radius * 3) + 0.5, 1);
    FragPos = aPos + vec3(aOffset * gap, 0) + vec3(0, 0, sin(wave + sin(wave / 10) * radius / 4) * 2);
    Normal = aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}