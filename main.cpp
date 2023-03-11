#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "getfilenames.h"
#include "stb_image.h"
#include "Shader.h"
#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"
#include "./imgui/imgui_stdlib.h"

float x_offset = 0.0f;
float y_offset = 0.0f;
float sv = 1.0f;
float lastX = 600.0f;
float lastY = 450.0f;
float lastTime = 0.0f;
float theta = 0.0f;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

std::string filePath = "./resource";
std::vector<std::string> filenames;

glm::mat4 trans(1.0f);

unsigned int ID;
unsigned int picture_number = 0;

int Index = 0;
int shadow = 0;
int Count_Rotate = 0;

bool firstMouse;
bool whether_go_back = true;
bool ShouldDrawBottomDocker = false;
bool ShouldDrawScaleDocker = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* windwo, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void LoadImage(int &width, int &height, int &nrChannels);
void ImageRotate();
void DrawBottomDocker();
void DrawBottomDockerShadow();
void DrawScaleDocker();

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PictureViewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader Shader("./Shader/vShader.glsl", "./Shader/fShader.glsl");
    ID = Shader.ID;

    float vertices[] = {
        //顶点坐标     //纹理坐标
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // 右上
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 右下
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 左下
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // 左上
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    GetFileNames(filePath, filenames);
    picture_number = filenames.size();
    stbi_set_flip_vertically_on_load(true);

    //Set imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;

    io.Fonts->AddFontFromFileTTF("./Fonts/Hack Regular Nerd Font Complete.ttf", 30, NULL, io.Fonts->GetGlyphRangesChineseFull());

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(whether_go_back){
            //设置纹理
            trans = glm::mat4(1.0f);
            x_offset = 0.0f; y_offset = 0.0f;
            Shader.use();
            LoadImage(width, height, nrChannels);
            Shader.setFloat("xoffset", x_offset);
            Shader.setFloat("yoffset", y_offset);
            float scale_value = 1.0f;
            if(width * 3 > height * 4){
                scale_value = ((float)height / width) * ((float)SCR_WIDTH / SCR_HEIGHT);
                trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
            }
            else if(width * 3 < height * 4){
                scale_value = ((float)width / height) * ((float)SCR_HEIGHT / SCR_WIDTH);
                trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
            }
            std::cout << scale_value << std::endl;
            whether_go_back = false;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        Shader.use();
        Shader.setFloat("theta", theta);
        glUniformMatrix4fv(glGetUniformLocation(ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ShouldDrawBottomDocker){
            DrawBottomDocker();
            DrawBottomDockerShadow();
        }

        if(ShouldDrawScaleDocker){
            float currentTime = glfwGetTime();
            if(currentTime - lastTime > 1.0){
                ShouldDrawScaleDocker = false;
            }
            DrawScaleDocker();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        y_offset -= 0.007f;
        glUniform1f(glGetUniformLocation(ID, "yoffset"), y_offset);
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        y_offset += 0.007f;
        glUniform1f(glad_glGetUniformLocation(ID, "yoffset"), y_offset);
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        Index = (Index + picture_number - 1) % picture_number;
        whether_go_back = true;
        std::cout << "whether_go_back:" << whether_go_back << " Index:" << Index << std::endl;
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        Index = (Index + 1) % picture_number;
        whether_go_back = true;
        std::cout << "whether_go_back:" << whether_go_back << " Index:" << Index << std::endl;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    float scale_value = 1.0f;
    scale_value += (float)yoffset * 0.05;
    if((int)yoffset){
        ShouldDrawScaleDocker = true;
        std::cout << "ShouldDrawScaleDocker\n";
        lastTime = glfwGetTime();
    }
    if(sv * scale_value < 0.2f || sv * scale_value > 20.0f){
        trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
        return;
    }
    sv *= scale_value;
    trans = glm::scale(trans, glm::vec3(scale_value, scale_value, scale_value));
    std::cout << "scale_value:" << sv << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        firstMouse = true;
        if((int)xpos > 240 && (int)xpos < 960 && (int)ypos > 800 && (int)ypos < 900){
            ShouldDrawBottomDocker = true;
            if((int)xpos > 285 && (int)xpos < 365 && (int)ypos > 805 && (int)ypos < 895)
                shadow = 1;
            else if((int)xpos > 395 && (int)xpos < 475 && (int)ypos > 810 && (int)ypos < 890)
                shadow = 2;
            else if((int)xpos > 505 && (int)xpos < 585 && (int)ypos > 810 && (int)ypos < 890)
                shadow = 3;
            else if((int)xpos > 615 && (int)xpos < 695 && (int)ypos > 810 && (int)ypos < 890)
                shadow = 4;
            else if((int)xpos > 725 && (int)xpos < 805 && (int)ypos > 810 && (int)ypos < 890)
                shadow = 5;
            else if((int)xpos > 835 && (int)xpos < 915 && (int)ypos > 810 && (int)ypos < 890)
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
                ShouldDrawScaleDocker = true;
                lastTime = glfwGetTime();
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
                ShouldDrawScaleDocker = true;
                lastTime = glfwGetTime();
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void LoadImage(int& width, int& height, int& nrChannels){
    unsigned char* data;
    data = stbi_load(filenames[Index].c_str(), &width, &height, &nrChannels, 0);
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
        int width, height, nrChannels;
        LoadImage(width, height, nrChannels);
        float scale_value = 1.0f;
        if(width * 3 < height * 4){
            scale_value = ((float)width / height) * ((float)SCR_WIDTH / SCR_HEIGHT);
            trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
            std::cout << 1.0f / scale_value << " " << (float)height / width << "\n111\n";
        }
        else if(width * 3 > height * 4){
            scale_value = ((float)height / width) * ((float)SCR_HEIGHT / SCR_WIDTH);
            trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
            std::cout << "222\n";
        }
        else{
            scale_value = ((float)height / width) * ((float)SCR_HEIGHT / SCR_WIDTH);
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
            scale_value = ((float)height / width) * ((float)SCR_WIDTH / SCR_HEIGHT);
            trans = glm::scale(trans, glm::vec3(1.0f, scale_value, scale_value));
        }
        else if(width * 3 < height * 4){
            scale_value = ((float)width / height) * ((float)SCR_HEIGHT / SCR_WIDTH);
            trans = glm::scale(trans, glm::vec3(scale_value, 1.0f, scale_value));
        }
        trans = glm::scale(trans, glm::vec3(sv));
    }
}

void DrawBottomDocker(){
    ImColor IconColor(0, 0, 0, 220);
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    DrawList->AddRectFilled(ImVec2(240, 800), ImVec2(960, 900), ImColor(255, 255, 255, 220), 20.0f);
    //绘制控件图标
    //左旋
    DrawList->PathArcTo(ImVec2(325, 850), 28.0f, glm::radians(210.0f), glm::radians(450.0f));
    DrawList->PathStroke(IconColor, 0, 5.0f);
    DrawList->PathClear();
    DrawList->AddTriangleFilled(ImVec2(294.5, 850), ImVec2(297.5, 820), ImVec2(302.36, 837.5), IconColor);
    //右旋
    DrawList->PathArcTo(ImVec2(435, 850), 28.0f, glm::radians(90.0f), glm::radians(330.0f));
    DrawList->PathStroke(IconColor, 0, 5.0f);
    DrawList->PathClear();
    DrawList->AddTriangleFilled(ImVec2(462.5, 820), ImVec2(465.5, 850), ImVec2(457.64, 837.5), IconColor);
    //向左翻页
    DrawList->AddCircle(ImVec2(545, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(530, 850), ImVec2(560, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(545, 835), ImVec2(531, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(545, 865), ImVec2(531, 850), IconColor, 5.0f);
    //向右翻页
    DrawList->AddCircle(ImVec2(655, 850), 30.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(640, 850), ImVec2(670, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(668, 850), ImVec2(655, 835), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(668, 850), ImVec2(655, 865), IconColor, 5.0f);
    //放大
    DrawList->AddCircle(ImVec2(765, 850), 25.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(782.6, 867.6), ImVec2(795, 880), IconColor, 5.0f);
    //"+"
    DrawList->AddLine(ImVec2(752.5, 850), ImVec2(777.5, 850), IconColor, 5.0f);
    DrawList->AddLine(ImVec2(765, 837.5), ImVec2(765, 862.5), IconColor, 5.0f);
    //缩小
    DrawList->AddCircle(ImVec2(875, 850), 25.0f, IconColor, 0, 5.0f);
    DrawList->AddLine(ImVec2(892.6, 867.6), ImVec2(905, 880), IconColor, 5.0f);
    //"-"
    DrawList->AddLine(ImVec2(862.5, 850), ImVec2(887.5, 850), IconColor, 5.0f);
}

void DrawBottomDockerShadow(){
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    if(shadow == 1)
        DrawList->AddRectFilled(ImVec2(285, 810), ImVec2(365, 890), ImColor(0, 0, 0, 100));
    if(shadow == 2)
        DrawList->AddRectFilled(ImVec2(395, 810), ImVec2(475, 890), ImColor(0, 0, 0, 100)); 
    if(shadow == 3) 
        DrawList->AddRectFilled(ImVec2(505, 810), ImVec2(585, 890), ImColor(0, 0, 0, 100)); 
    if(shadow == 4) 
        DrawList->AddRectFilled(ImVec2(615, 810), ImVec2(695, 890), ImColor(0, 0, 0, 100));
    if(shadow == 5)
        DrawList->AddRectFilled(ImVec2(725, 810), ImVec2(805, 890), ImColor(0, 0, 0, 100));
    if(shadow == 6)
        DrawList->AddRectFilled(ImVec2(835, 810), ImVec2(915, 890), ImColor(0, 0, 0, 100));
}

void DrawScaleDocker(){
    ImColor IconColor(0, 0, 0, 220);
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    DrawList->AddRectFilled(ImVec2(540, 420), ImVec2(660, 480), ImColor(255, 255, 255, 220), 10.0f);
    //绘制图标
    DrawList->AddCircle(ImVec2(565, 450), 15, IconColor, 0, 3.0f);
    DrawList->AddLine(ImVec2(575.6, 460.6), ImVec2(585, 470), IconColor, 3.0f);
    //"+"
    DrawList->AddLine(ImVec2(557.5, 450), ImVec2(572.5, 450), IconColor, 3.0f);
    DrawList->AddLine(ImVec2(565, 442.5), ImVec2(565, 457.5), IconColor, 3.0f);
    //缩放程度
    int Scale_Value = sv * 100;
    std::string output = std::to_string(Scale_Value);
    output += "%";
    if(Scale_Value < 100)
        DrawList->AddText(ImVec2(600, 435), ImColor(0, 0, 0), output.c_str());
    else if(Scale_Value > 1000)
        DrawList->AddText(ImVec2(583, 435), ImColor(0, 0, 0), output.c_str());
    else
        DrawList->AddText(ImVec2(590, 435), ImColor(0, 0, 0), output.c_str());
}