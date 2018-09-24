#include "resources.h"
#include "resources_binaries.h"
#include <learnopengl/filesystem.h>
#include <stb_image.h>
#include <fstream>
#include <sstream>


struct binary_resource {
	unsigned char* data;
	unsigned int length;
};

ty::result<binary_resource> get_binary_resource(const std::string& path) {
	binary_resource res;
	if (false);
	else if (path == "resources/textures/info.png"   ) { res.data = info_png   ; res.length = info_png_len   ; }
	else if (path == "resources/textures/wwix_00.png") { res.data = wwix_00_png; res.length = wwix_00_png_len; }
	else if (path == "resources/textures/wwix_01.png") { res.data = wwix_01_png; res.length = wwix_01_png_len; }
	else if (path == "resources/textures/wwix_02.png") { res.data = wwix_02_png; res.length = wwix_02_png_len; }
	else if (path == "resources/textures/wwix_03.png") { res.data = wwix_03_png; res.length = wwix_03_png_len; }
	else if (path == "resources/textures/wwix_04.png") { res.data = wwix_04_png; res.length = wwix_04_png_len; }
	else if (path == "resources/textures/wwix_05.png") { res.data = wwix_05_png; res.length = wwix_05_png_len; }
	else if (path == "resources/textures/wwix_06.png") { res.data = wwix_06_png; res.length = wwix_06_png_len; }
	else if (path == "resources/textures/wwix_07.png") { res.data = wwix_07_png; res.length = wwix_07_png_len; }
	else if (path == "resources/textures/wwix_08.png") { res.data = wwix_08_png; res.length = wwix_08_png_len; }
	else if (path == "resources/textures/wwix_09.png") { res.data = wwix_09_png; res.length = wwix_09_png_len; }
	else if (path == "resources/textures/wwix_10.png") { res.data = wwix_10_png; res.length = wwix_10_png_len; }
	else if (path == "resources/textures/wwix_11.png") { res.data = wwix_11_png; res.length = wwix_11_png_len; }
	else if (path == "resources/textures/wwix_12.png") { res.data = wwix_12_png; res.length = wwix_12_png_len; }
	else if (path == "resources/textures/wwix_13.png") { res.data = wwix_13_png; res.length = wwix_13_png_len; }
	else if (path == "resources/textures/wwix_14.png") { res.data = wwix_14_png; res.length = wwix_14_png_len; }
	else if (path == "resources/textures/wwix_15.png") { res.data = wwix_15_png; res.length = wwix_15_png_len; }
	else if (path == "resources/textures/wwix_16.png") { res.data = wwix_16_png; res.length = wwix_16_png_len; }
	else if (path == "resources/textures/wwix_17.png") { res.data = wwix_17_png; res.length = wwix_17_png_len; }
	else if (path == "resources/textures/wwix_18.png") { res.data = wwix_18_png; res.length = wwix_18_png_len; }
	else if (path == "resources/textures/wwix_19.png") { res.data = wwix_19_png; res.length = wwix_19_png_len; }
	else if (path == "resources/textures/wwix_20.png") { res.data = wwix_20_png; res.length = wwix_20_png_len; }
	else if (path == "resources/textures/wwix_21.png") { res.data = wwix_21_png; res.length = wwix_21_png_len; }
	else if (path == "resources/textures/wwix_22.png") { res.data = wwix_22_png; res.length = wwix_22_png_len; }
	else if (path == "resources/textures/wwix_23.png") { res.data = wwix_23_png; res.length = wwix_23_png_len; }
	else if (path == "resources/textures/wwix_24.png") { res.data = wwix_24_png; res.length = wwix_24_png_len; }
	else if (path == "resources/textures/wwix_25.png") { res.data = wwix_25_png; res.length = wwix_25_png_len; }
	else if (path == "resources/textures/wwix_26.png") { res.data = wwix_26_png; res.length = wwix_26_png_len; }
	else if (path == "resources/textures/wwix_27.png") { res.data = wwix_27_png; res.length = wwix_27_png_len; }
	else if (path == "resources/textures/wwix_28.png") { res.data = wwix_28_png; res.length = wwix_28_png_len; }
	else if (path == "resources/textures/wwix_29.png") { res.data = wwix_29_png; res.length = wwix_29_png_len; }
	else if (path == "resources/textures/wwix_30.png") { res.data = wwix_30_png; res.length = wwix_30_png_len; }
	else if (path == "resources/textures/wwix_31.png") { res.data = wwix_31_png; res.length = wwix_31_png_len; }
	else if (path == "resources/textures/wwix_32.png") { res.data = wwix_32_png; res.length = wwix_32_png_len; }
	else if (path == "resources/textures/wwix_33.png") { res.data = wwix_33_png; res.length = wwix_33_png_len; }
	else if (path == "resources/textures/wwix_34.png") { res.data = wwix_34_png; res.length = wwix_34_png_len; }
	else if (path == "resources/textures/wwix_35.png") { res.data = wwix_35_png; res.length = wwix_35_png_len; }
	else if (path == "resources/textures/wwix_36.png") { res.data = wwix_36_png; res.length = wwix_36_png_len; }
	else if (path == "resources/textures/wwix_37.png") { res.data = wwix_37_png; res.length = wwix_37_png_len; }
	else if (path == "resources/textures/wwix_38.png") { res.data = wwix_38_png; res.length = wwix_38_png_len; }
	else if (path == "resources/textures/wwix_39.png") { res.data = wwix_39_png; res.length = wwix_39_png_len; }
	else if (path == "resources/textures/wwix_40.png") { res.data = wwix_40_png; res.length = wwix_40_png_len; }
	else if (path == "resources/textures/wwix_41.png") { res.data = wwix_41_png; res.length = wwix_41_png_len; }
	else if (path == "resources/textures/wwix_42.png") { res.data = wwix_42_png; res.length = wwix_42_png_len; }
	else if (path == "resources/textures/wwix_43.png") { res.data = wwix_43_png; res.length = wwix_43_png_len; }
	else if (path == "resources/textures/wwix_44.png") { res.data = wwix_44_png; res.length = wwix_44_png_len; }
	else if (path == "resources/textures/wwix_45.png") { res.data = wwix_45_png; res.length = wwix_45_png_len; }
	else {
		RETURN_ERROR("dont find resource binary: " + path)
	}
	return res;
}


Image_Data::Image_Data(unsigned char* data, int width, int height, int nr_channels) : 
	buffer{ data, stbi_image_free }, 
	width{ width }, height{ height }, nr_channels{ nr_channels } {}

unsigned char* Image_Data::get_data() const {
	return buffer.get();
}

ty::result<Image_Data> Image_Data::load(const char* path, int desired_channels) {
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	int width, height, nr_channels;
#if DEBUG
	auto data = stbi_load(FileSystem::getPath(path).c_str(), &width, &height, &nr_channels, desired_channels);
#else
	TRY_RESULT(auto, binary, get_binary_resource(path));
	auto data = stbi_load_from_memory(binary.data, binary.length, &width, &height, &nr_channels, desired_channels);
#endif
	if (data == nullptr) {
	    return ty::error{TY_INTERNAL_FILE_CTX + " image not found: " + path};
	}
	Image_Data image{
		data,
		width, height, nr_channels
	};
	return std::move(image);
}

ty::result<std::string> get_shader_from_file(const char* path) {
	auto vertexPathString = FileSystem::getPath(path);
	auto vertexPath = vertexPathString.c_str();
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::ifstream vShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		std::stringstream vShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		RETURN_ERROR("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	return vertexCode;
}

ty::result<const char*> get_shader_from_memory(const std::string& path) {
	const char* result = nullptr;
	if (false);
	else if (path == "resources/shaders/info_panel.fs") result = info_panel_fs;
	else if (path == "resources/shaders/info_panel.vs") result = info_panel_vs;
	else if (path == "resources/shaders/voxel.fs") result = voxel_fs;
	else if (path == "resources/shaders/voxel.vs") result = voxel_vs;
	if (result == nullptr) {
		RETURN_ERROR("shader not fund"+ path);
	} else {
		return result;
	}
}

ty::result<Shader> Shader::load_shader(const char* vertexFilename, const char* fragmentFilename) {
	Shader shader;
	shader.empty = false;
#if DEBUG
	TRY_RESULT(std::string, vertexCode, get_shader_from_file(vertexFilename));
	TRY_RESULT(std::string, fragmentCode, get_shader_from_file(fragmentFilename));
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
#else
	TRY_RESULT(const char*, vShaderCode, get_shader_from_memory(vertexFilename));
	TRY_RESULT(const char*, fShaderCode, get_shader_from_memory(fragmentFilename));
#endif
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	TRY_ERROR(shader.checkCompileErrors(vertex, "VERTEX"));
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	TRY_ERROR(shader.checkCompileErrors(fragment, "FRAGMENT"));
	// shader Program
	shader.ID = glCreateProgram();
	glAttachShader(shader.ID, vertex);
	glAttachShader(shader.ID, fragment);
	glLinkProgram(shader.ID);
	TRY_ERROR(shader.checkCompileErrors(shader.ID, "PROGRAM"));
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return shader;
}
// activate the shader
// ------------------------------------------------------------------------
ty::error Shader::use() const
{
	if (empty) {
		RETURN_ERROR("Trying to use empty shader!");
	}
	glUseProgram(ID);
	RETURN_OK;
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
ty::error Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::stringstream ss;
			ss << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			RETURN_ERROR(ss.str());
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::stringstream ss;
			ss << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			RETURN_ERROR(ss.str());
		}
	}
	RETURN_OK;
}
