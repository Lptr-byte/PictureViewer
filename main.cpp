#define STB_IMAGE_IMPLEMENTATION
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <ostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include "Shader.h"
#include "stb_image.h"
#include "getfilenames.h"
#include "char_string.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"

float mouse_x = 0.0f;
float mouse_y = 0.0f;
float x_offset = 0.0f;
float y_offset = 0.0f;
float theta = 0.0f;
float sv = 1.0f;
float lastX = 1200.0f, lastY = 450.0f;
float lastTime = 0.0f;

bool firstMouse;

unsigned int ID;

glm::mat4 trans(1.0f);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;
const unsigned int MAXN = 10000;

std::string filePath = "./resource";
std::vector<std::string> filenames;
int Index = 0, picture_number = 0;
int shadow = 0;
int Count_Rotate = 0;
//int BottomDockerIconCenterX[] = {325, 435, 545, 655, 765, 875};
//int BottomDockerIconCenterY = 850;
//int ShadowHalfWidth = 45, ShadowHalfHeight = 45;
bool firstOpenApp = true;
bool whether_go_back = true;
bool ShouldDrawBottomDocker = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* windwo, double xpos, double ypos);
void processInput(GLFWwindow* window, Shader shader);
void DrawBottomDocker();
void DrawBottomDockerShadow();
void LoadImage(int &width, int &height, int &nrChannel);
void ImageRotate();

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PictureViewer", nullptr, nullptr);
    if(window == nullptr){
        std::cout << "Create Window Fail!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed initized glad!\n";
        return 0;
    } 

    //Set imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;

    ImGui::StyleColorsDark();

    Shader shader("./Shader/vShader.txt", "./Shader/fShader.txt");
    //Shader shader1("./Shader/vShader.txt", "./Shader/fShader.txt");
    ID = shader.ID;
    
    //创建顶点
    float vertices[] = {
        //顶点坐标          //顶点颜色         //纹理坐标
        -1.0,  1.0, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   //左上
         1.0,  1.0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,   //右上
        -1.0, -1.0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   //左下
         1.0, -1.0, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f    //右下
    };
    unsigned int indeces[] = {
        0, 1, 2,
        1, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点读入缓冲区
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   //0即设定的位置"location=0"
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int textrue, textrue1;
    glGenTextures(1, &textrue);
    glBindTexture(GL_TEXTURE_2D, textrue);

    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannel;
    GetFileNames(filePath, filenames);
    picture_number = filenames.size(); 

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    while(!glfwWindowShouldClose(window)){
        processInput(window, shader);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(whether_go_back){
            //设置纹理
            trans = glm::mat4(1.0f);
            x_offset = 0.0f; y_offset = 0.0f;
            theta = 0.0f;
            shader.use();
            LoadImage(width, height, nrChannel);
            shader.setFloat("xoffset", x_offset);
            shader.setFloat("yoffset", y_offset);
            float scale_value = 1.0f;
            if(width * 3 > height * 4){
                scale_value = (float)height * SCR_WIDTH / (width * SCR_HEIGHT);
                trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
            }
            else if(width * 3 < height * 4){
                scale_value = (float)width * SCR_HEIGHT / (height * SCR_WIDTH);
                trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
            }
            std::cout << scale_value << std::endl;
            whether_go_back = false;
        }

        glUniform1f(glGetUniformLocation(shader.ID, "theta"), theta);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textrue);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ShouldDrawBottomDocker){
            DrawBottomDocker();
            DrawBottomDockerShadow();
        }
        /*
        ImGui::Begin(" ");
    
        if(ImGui::InputText("Please input file path:", &filePath)){
            filenames.clear();
            GetFileNames(filePath, filenames);
            picture_number = filenames.size(); 
        }
        ImGui::End();
        */
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader.ID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, Shader shader){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        y_offset -= 0.005f;
        shader.setFloat("yoffset", y_offset);
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        y_offset += 0.005f;
        shader.setFloat("yoffset", y_offset);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        int currentTime = glfwGetTime();
        if(currentTime - lastTime < 0.000000001f){
            lastTime = currentTime;
            return;
        }
        std::cout << "Delta Time:" << currentTime - lastTime << std::endl;
        lastTime = currentTime;
        Index = (Index + picture_number - 1) % picture_number;
        whether_go_back = true;
        std::cout << "whether_go_back:" << whether_go_back << " Index:" << Index << std::endl;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        int currentTime = glfwGetTime();
        if(currentTime - lastTime < 0.000000001f){
            lastTime = currentTime;
            return;
        }
        std::cout << "whether_go_back:" << whether_go_back << " Index:" << Index << std::endl;
        lastTime = currentTime;
        Index = (Index + 1) % picture_number;
        whether_go_back = true;
        std::cout << "whether_go_back:" << whether_go_back << " Index:" << Index << std::endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    float scale_value = 1.0f;
    scale_value += (float)yoffset * 0.05;
    if(sv * scale_value < 0.2f || sv * scale_value > 20.0f){
        trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
        return;
    }
    sv *= scale_value;
    trans = glm::scale(trans, glm::vec3(scale_value, scale_value, scale_value));
    std::cout << "scale_value:" << sv << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    mouse_x = (float)xpos; mouse_y = (float)ypos;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        firstMouse = true;
        if((int)xpos > 240 && (int)xpos < 960 && (int)ypos > 800 && (int)ypos < 900){
            ShouldDrawBottomDocker = true;
            if((int)xpos > 280 && (int)xpos < 370 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 1;
            else if((int)xpos > 390 && (int)xpos < 480 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 2;
            else if((int)xpos > 500 && (int)xpos < 590 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 3;
            else if((int)xpos > 610 && (int)xpos < 700 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 4;
            else if((int)xpos > 720 && (int)xpos < 810 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 5;
            else if((int)xpos > 830 && (int)xpos < 920 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 6;
            else
                shadow = 0;
        }
        else {
            ShouldDrawBottomDocker = false;
        }
        std::cout << "xpos:" << xpos << " ypos:" << ypos << std::endl;
        return;
    }
    std::cout << "xpos:" << xpos << " ypos:" << ypos << std::endl;
    if(firstMouse){
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float deltaX = (lastX - (float)xpos) * 0.002f;
    float deltaY = ((float)ypos - lastY) * 0.002f;
    x_offset -= deltaX;
    y_offset -= deltaY;
    lastX = (float)xpos;
    lastY = (float)ypos;
    glUniform1f(glGetUniformLocation(ID, "xoffset"), x_offset);
    glUniform1f(glGetUniformLocation(ID, "yoffset"), y_offset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //处理鼠标点击控件事件
    if(action == GLFW_PRESS){
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            std::cout << "Mouse Button Left Down!\n";
            std::cout << "mouse_x:" << mouse_x << " mouse_y:" << mouse_y << std::endl;
            //左旋
            if(shadow == 1){
                Count_Rotate++;
                theta += glm::radians(90.0f);
                ImageRotate();                
            }
            //右旋
            if(shadow == 2){
                Count_Rotate++;
                theta -= glm::radians(90.0f);
                ImageRotate();
            }
            //向左翻页
            if(shadow == 3){
                Index = (Index + picture_number - 1) % picture_number;
                theta = 0.0f;
                whether_go_back = true;
            }
            //向右翻页
            if(shadow == 4){
                Index = (Index + 1) % picture_number;
                theta = 0.0f;
                whether_go_back = true;
            }
            //放大
            if(shadow == 5){
                float scale_value = 1.0f;
                scale_value += 0.05;
                if(sv * scale_value < 0.2f || sv * scale_value > 20.0f){
                    trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
                    return;
                }
                sv *= scale_value;
                trans = glm::scale(trans, glm::vec3(scale_value, scale_value, scale_value));
                std::cout << "scale_value:" << sv << std::endl;
            }
            //缩小
            if(shadow == 6){
                float scale_value = 1.0f;
                scale_value -= 0.05;
                if(sv * scale_value < 0.2f || sv * scale_value > 20.0f){
                    trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
                    return;
                }
                sv *= scale_value;
                trans = glm::scale(trans, glm::vec3(scale_value, scale_value, scale_value));
                std::cout << "scale_value:" << sv << std::endl;
            }
        }
    }
}

void DrawBottomDocker(){
    ImColor IconColor(0, 0, 0, 220);
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    DrawList->AddRectFilled(ImVec2(240, 800), ImVec2(960, 900), ImColor(255, 255, 255, 220), 20.0f);
    //绘制控件图标
    //左旋
    DrawList->PathArcTo(ImVec2(325, 850), 30.0f, glm::radians(180.0f), glm::radians(450.0f));
    DrawList->PathStroke(IconColor, 0, 5.0f);
    DrawList->PathClear();
    DrawList->AddTriangleFilled(ImVec2(292.5, 850), ImVec2(292.5, 820), ImVec2(300.36, 830), IconColor);
    //右旋
    DrawList->PathArcTo(ImVec2(435, 850), 30.0f, glm::radians(90.0f), glm::radians(360.0f));
    DrawList->PathStroke(IconColor, 0, 5.0f);
    DrawList->PathClear();
    DrawList->AddTriangleFilled(ImVec2(467.5, 850), ImVec2(467.5, 820), ImVec2(459.64, 830), IconColor);
    //向左翻页
    DrawList->AddCircle(ImVec2(545, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(530, 850), ImVec2(560, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(545, 835), ImVec2(531, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(545, 865), ImVec2(531, 850), IconColor, 5.0f);
    //向右翻页
    DrawList->AddCircle(ImVec2(655, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(640, 850), ImVec2(670, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(669, 850), ImVec2(655, 835), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(669, 850), ImVec2(655, 865), IconColor, 5.0f);
    //放大
    DrawList->AddCircle(ImVec2(765, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(805, 890), ImVec2(786.2, 871.2), IconColor, 5.0f);
    //"+"
    DrawList->AddLine(ImVec2(745, 850), ImVec2(785, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(765, 830), ImVec2(765, 870), IconColor, 5.0f);
    //缩小
    DrawList->AddCircle(ImVec2(875, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(915, 890), ImVec2(896.2, 871.2), IconColor, 5.0f);
    //"-"
    DrawList->AddLine(ImVec2(855, 850), ImVec2(895, 850), IconColor, 5.0f);
}

void DrawBottomDockerShadow(){
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();

    if(shadow == 1)
        DrawList->AddRectFilled(ImVec2(280, 805), ImVec2(370, 895), ImColor(0, 0, 0, 100));
    if(shadow == 2)
        DrawList->AddRectFilled(ImVec2(390, 805), ImVec2(480, 895), ImColor(0, 0, 0, 100)); if(shadow == 3) DrawList->AddRectFilled(ImVec2(500, 805), ImVec2(590, 895), ImColor(0, 0, 0, 100)); if(shadow == 4) DrawList->AddRectFilled(ImVec2(610, 805), ImVec2(700, 895), ImColor(0, 0, 0, 100));
    if(shadow == 5)
        DrawList->AddRectFilled(ImVec2(720, 805), ImVec2(810, 895), ImColor(0, 0, 0, 100));
    if(shadow == 6)
        DrawList->AddRectFilled(ImVec2(830, 805), ImVec2(920, 895), ImColor(0, 0, 0, 100));
}

void LoadImage(int& width, int& height, int& nrChannel){
   //unsigned char* data = stbi_load(".//resource//1.jpg", &width, &height, &nrChannel, 0);
    unsigned char* data;
    data = stbi_load(filenames[Index].c_str(), &width, &height, &nrChannel, 0);
    std::cout << filenames[Index] << std::endl;
    std::cout << "Index:" << Index << std::endl;
    if(data){
        if(filenames[Index].find(".jpg") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if(filenames[Index].find(".png") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Load Texture Failed!\n";
    }
    stbi_image_free(data);
    std::cout << "Width = " << width << " Height = " << height << "\n";

    glUniform1i(glGetUniformLocation(ID, "myTexture1"), 0);
}

void ImageRotate(){
    if(Count_Rotate % 2 == 1){
        trans = glm::mat4(1.0f);
        int width, height, nrChannel;
        LoadImage(width, height, nrChannel);
        float scale_value = 1.0f;
        if(width * 3 < height * 4){
            scale_value = (float)width * SCR_WIDTH / (height * SCR_HEIGHT);
            trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
            std::cout << 1.0f / scale_value << " " << (float)height / width << "\n111\n";
        }
        else if(width * 3 > height * 4){
            scale_value = (float)height * SCR_HEIGHT / (width * SCR_WIDTH);
            trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
            std::cout << "222\n";
        }
        else{
            scale_value = (float)height * SCR_HEIGHT / (width * SCR_WIDTH);
            trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));

        }
        trans = glm::scale(trans, glm::vec3(sv));
    }
    else{
        trans = glm::mat4(1.0f);
        int width, height, nrChannel;
        LoadImage(width, height, nrChannel);
        float scale_value = 1.0f;
        if(width * 3 > height * 4){
            scale_value = (float)height * SCR_WIDTH / (width * SCR_HEIGHT);
            trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
        }
        else if(width * 3 < height * 4){
            scale_value = (float)width * SCR_HEIGHT / (height * SCR_WIDTH);
            trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
        }
        trans = glm::scale(trans, glm::vec3(sv));
    }
}