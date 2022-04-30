//
// Created by Christina Zhang on 2022-04-21.
//

#ifndef OPENGL_PRACTICE_SHADER_H
#define OPENGL_PRACTICE_SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID;

    Shader (const char * vertexPath, const char * fragmentPath);

    void use();
    void del();

    // for uniforms
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec4(const std::string &name, float v1, float v2, float v3, float v4) const;
};

#endif //OPENGL_PRACTICE_SHADER_H
