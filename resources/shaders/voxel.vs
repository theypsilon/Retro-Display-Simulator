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
uniform vec3 voxel_scale;
uniform float pulse;

void main()
{
	float radius = length(aOffset);
	ObjectColor = aColor;
    FragPos = aPos / voxel_scale + vec3(aOffset, 0) + vec3(0, 0, sin(pulse + sin(pulse / 10) * radius / 4) * 2);
    Normal = aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}