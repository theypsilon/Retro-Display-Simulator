#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
uniform float wave;

void main()
{
	vec4 result = texture(texture1, TexCoord);
	if (result.a == 0.0) {
		discard;
	}
	float red   = sin(wave * 10) * 0.3 + 0.7;
	float green = sin(wave * 15) * 0.3 + 0.7;
	float blue  = sin(wave * 20) * 0.3 + 0.7;
	FragColor = vec4(red, blue, green, 1);
}