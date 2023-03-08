#pragma once
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
public:
    unsigned int ID;

    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;
    void setMatrix4fv(const std::string &name, glm::mat4 value) const;
};

inline Shader::Shader(const char* vertexShaderPath, const char* framentShaderPath){
   std::string vertexCode;
   std::string fragmentCode;
   std::ifstream vertexShaderFile;
   std::ifstream fragmentShaderFile; 
   vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
   
   try{
        vertexShaderFile.open(vertexShaderPath);
        fragmentShaderFile.open(framentShaderPath);

        std::stringstream vShader, fShader;
        vShader << vertexShaderFile.rdbuf();
        fShader << fragmentShaderFile.rdbuf();

        vertexCode = vShader.str();
        fragmentCode = fShader.str();
   }catch(std::ifstream::failure e){
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
   }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //创建并编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    //检查报错
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //创建并编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    //检查报错
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //链接着色器
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    //检查链接
    glGetShaderiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    //删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

inline void Shader::use(){
    glUseProgram(ID);
}

inline void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

inline void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

inline void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

inline void Shader::setMatrix4fv(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}