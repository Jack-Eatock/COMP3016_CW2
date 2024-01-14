#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	std::string type;
	std::string fileDir;
	GLuint unit;

	Texture() {};

	Texture(std::string image, std::string texType, GLuint slot, bool flip = false);

	void SetupTexture(std::string image, std::string texType, GLuint slot, bool flip = false);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif
