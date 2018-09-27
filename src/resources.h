#ifndef RESOURCES
#define RESOURCES

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <theypsilon/error.h>
#include <memory>
#include <functional>

class Image_Data {
    Image_Data(unsigned char* data, int width, int height, int nr_channels);
    std::unique_ptr<unsigned char, std::function<void(unsigned char*)>> buffer;
public:
    unsigned char* get_data() const;
    static ty::result<Image_Data> load(const char* path, int desired_channels);
    const int width, height, nr_channels;
};

class Shader
{
    bool empty = true;
public:
    unsigned int ID;
    Shader() = default;

    static ty::result<Shader> load_shader(const char* vertexPath, const char* fragmentPath);
    ty::error use() const;
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec2(const char* name, const glm::vec2 &value) const;
    void setVec2(const char* name, float x, float y) const;
    void setVec3(const char* name, const glm::vec3 &value) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setVec4(const char* name, const glm::vec4 &value) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
    void setMat2(const char* name, const glm::mat2 &mat) const;
    void setMat3(const char* name, const glm::mat3 &mat) const;
    void setMat4(const char* name, const glm::mat4 &mat) const;

private:
    ty::error checkCompileErrors(GLuint shader, std::string type);
};

#endif