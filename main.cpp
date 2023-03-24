//#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "LoadTextureFromFile.h"
#include "getfilenames.h"
#include "stb_image.h"
#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"
#include "./imgui/imgui_stdlib.h"
#include "./ImGuiFileDialog/ImGuiFileDialog.h"
#include "./ImGuiFileDialog/CustomFont.h"

float mouse_x = 0.0f;
float mouse_y = 0.0f;
float x_offset = 0.0f;
float y_offset = 0.0f;
float theta = 0.0f;
float sv = 1.0f;
float lastX = 600.0f, lastY = 450.0f;
float lastTime = 0.0f;

glm::mat4 trans(1.0f);

unsigned int ID;
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

std::string filePath = "./resource";
std::string filename, filePathName;
std::vector<std::string> filenames;

int Index = 0, picture_number = 0;
int shadow = 0;
int Count_Rotate = 0;

bool firstMouse = true;
bool firstOpenApp = true;
bool firstEnterFolder = true;
bool whether_go_back = true;
bool ShouldDrawBottomDocker = false;
bool ShouldDrawScaleDocker = false;
bool ShouldDrawMenu = true;
bool IsOpenInFolder = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* winddow, double xpos, double ypos);
void processInput(GLFWwindow* window, Shader shader);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void DrawBottomDocker();
void DrawBottomDockerShadow();
void DrawScaleDocker();
void LoadImage(int &width, int &height, int &nrChannel);
void ImageRotate();
void DrawMenu(GLFWwindow* window);

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
    glfwSetKeyCallback(window, key_callback);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed initized glad!\n";
        return 0;
    } 


    Shader shader("./Shader/vShader.glsl", "./Shader/fShader.glsl");
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

    unsigned int textrue;
    glGenTextures(1, &textrue);
    glBindTexture(GL_TEXTURE_2D, textrue);

    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannel;

    //Set imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImFont* MyFonts = io.Fonts->AddFontFromFileTTF("./Fonts/Hack Regular Nerd Font Complete.ttf", 20, NULL, io.Fonts->GetGlyphRangesDefault());
    static const ImWchar icons_ranges[] = { 0xeb99, 0xf0a88, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true; 
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("./Fonts/simhei.ttf", 20, &icons_config, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->AddFontFromFileTTF("./Fonts/Hack Regular Nerd Font Complete.ttf", 20, &icons_config, icons_ranges);
    io.Fonts->Build();

    io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10;
    style.FrameRounding = 7;
    style.WindowBorderSize = 0;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.13f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    while(!glfwWindowShouldClose(window)){
        processInput(window, shader);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textrue);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ShouldDrawMenu){
            DrawMenu(window);

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        if(whether_go_back){
            //设置纹理
            trans = glm::mat4(1.0f);
            x_offset = 0.0f; y_offset = 0.0f;
            theta = 0.0f;
            shader.use();
            GetFileNames(filePath, filenames);
            picture_number = filenames.size(); 
            if(!IsOpenInFolder){
                for(int i = 0; i < picture_number; i++){
                    if(filenames[i] == filePathName){
                        Index = i;
                        std::cout << "Index = " << Index << std::endl;
                        break;
                    }
                }
                IsOpenInFolder = true;
            }
            LoadImage(width, height, nrChannel);
            shader.setFloat("xoffset", x_offset);
            shader.setFloat("yoffset", y_offset);
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

        glUniform1f(glGetUniformLocation(shader.ID, "theta"), theta);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
    glDeleteProgram(shader.ID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, Shader shader){
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
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
    mouse_x = (float)xpos; mouse_y = (float)ypos;
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

void LoadImage(int& width, int& height, int& nrChannel){
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

void DrawMenu(GLFWwindow* window){
    ImGui::Begin(" ", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    //设置菜单信息
    ImDrawList* DrawList = ImGui::GetForegroundDrawList();
    ImDrawList* BackgroundDrawList = ImGui::GetBackgroundDrawList();
    ImColor textColor(255, 255, 255);
    ImFont* MyFont = ImGui::GetFont();
    //DrawList->AddText(MyFont, 150, ImVec2(30, 40), textColor, "Picture Viewer");
    
    //设置背景
    int my_image_width = 0;
    int my_image_height = 0;
    GLuint background = 0, BackgroundText = 0;
    bool ret = LoadTextureFromFile("./UI/background.png", &background, &my_image_width, &my_image_height);
    IM_ASSERT(ret);
    BackgroundDrawList->AddImage((void *)(intptr_t)background, ImVec2(0, 0), ImVec2(1200, 900));
    ret = LoadTextureFromFile("./UI/PictureViewer.png", &BackgroundText, &my_image_width, &my_image_height);
    IM_ASSERT(ret);
    BackgroundDrawList->AddImage((void*)(intptr_t)BackgroundText, ImVec2(0, 100), ImVec2(my_image_width, 100 + my_image_height)); 
    BackgroundDrawList->AddText(MyFont, 30, ImVec2(460, 860), textColor, "v2.0.0 by Hanasa");

    ImVec2 maxSize = ImVec2(SCR_WIDTH * 0.8, SCR_HEIGHT * 0.8);  // The full display area
    ImVec2 minSize = ImVec2(SCR_WIDTH * 0.5, SCR_HEIGHT * 0.5);  // Half the display area
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER); // for all dirs
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, nullptr, ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_CPP);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".c", ImVec4(1.0f, 1.0f, 0.6f, 0.9f), ICON_IGFD_C);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_HEADFILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_HPP);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_MARKDOWN);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC); // add an icon for the filter type
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), ICON_IGFD_FILE_PIC); // add an text for a filter type
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".ini", ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_SETTINGS); // add an text for a filter type
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);
    ImGuiFileDialog::Instance()->SetFlashingAttenuationInSeconds(1.0f);
    // open Dialog Simple
    if (ImGui::InvisibleButton(u8" 打开文件", ImVec2(120, 35)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileKey", u8"\uf07c Choose File", ".*,.jpg,.png", ".");
    if (ImGui::InvisibleButton(u8"打开文件夹", ImVec2(120, 35)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", u8"\uf07c Choose File", ".*", " ");
    if (ImGui::InvisibleButton(u8" 退出", ImVec2(120, 35)))
        glfwSetWindowShouldClose(window, true);

    // display
    if(ImGuiFileDialog::Instance()->Display("ChooseFileKey", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()){
            filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
            filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            // action
            std::cout << "fileName:" << filename << " filePath:" << filePath << '\n';
            if(filename.find(".jpg") != std::string::npos || filename.find(".png") != std::string::npos){
                ShouldDrawMenu = false;
                IsOpenInFolder = false;
            }
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
    
    if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk()){
            filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
            filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            // action
            std::cout << "fileName:" << filename << " filePath:" << filePath << '\n';
            ShouldDrawMenu = false;
            IsOpenInFolder = true;
            Index = 0;
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
