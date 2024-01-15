#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 model;

uniform float outlining;

void main()
{
	// First Method
	vec3 crntPos = vec3(model * outlining * vec4(aPos, 1.0f));
	gl_Position = camMatrix * vec4(crntPos, 1.0);

}