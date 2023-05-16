#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include<chrono>
#include<thread>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Includes/shader_s.h"
#include "camera.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow*, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void genGrid(int N, std::vector<glm::vec3>& vertices, std::vector<glm::uvec3>& indices);
GLuint generate_vao(const std::vector<glm::vec3>& vertices, const std::vector<glm::uvec3>& indices);
void draw_vao(GLuint vao, GLsizei n);



    //display size

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;



//camera init

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

float lastX = 400;
float lastY = 300;
bool firstMouse = true;


//highlight row and column indices
int curColumn = 0;
int curRow = 0;

bool shading = true;
bool liveData = false;




//ambient color
glm::vec3 ambientCol = glm::vec3(0.3f, 0.3f, 0.3f);


//timing

float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Morph", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    //glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //mouse input init
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
     

    

 

    //Shader Program ------------------------------------------
        
    //Shader ourShader("Shaders/morph.vs", "Shaders/fShader.fs");

    Shader ourShader("Shaders/morph.vs", "Shaders/fShader.fs", "Shaders/geom.gs");

    //add custom geometry shader




      
    // -------------------------------------------------------------------------------------------

    //light calcs 
    //Make 3 point Lighting

    ourShader.use();

    ourShader.setVec3("dirLight.direction", 0.3f, -0.5f, 0.0f);
    ourShader.setVec3("dirLight.ambient", ambientCol);
    ourShader.setVec3("dirLight.diffuse",glm::vec3(1.0f)); 
    ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);



    //Enable to use pointlights

    ourShader.use();
    

    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,  5.0f, -3.0f),
    glm::vec3(-3.0f, 5.0f, 3.0f),
    glm::vec3(3.0f, 5.0f, 3.0f),
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f),
        glm::vec3(0.5f),
        glm::vec3(0.2f),

    };

    for (int i = 0; i < 3; i++) {
        char buffer[64];
        sprintf_s(buffer, "pointLights[%i].position", i);
        ourShader.setVec3(buffer, pointLightPositions[i]);
        sprintf_s(buffer, "pointLights[%i].constant", i);
        ourShader.setFloat(buffer, 1.0f);
        sprintf_s(buffer, "pointLights[%i].linear", i);
        ourShader.setFloat(buffer, 0.09f);
        sprintf_s(buffer, "pointLights[%i].quadratic", i);
        ourShader.setFloat(buffer, 0.032f);

        sprintf_s(buffer, "pointLights[%i].ambient", i);
        ourShader.setVec3(buffer, ambientCol);
        sprintf_s(buffer, "pointLights[%i].diffuse", i);
        ourShader.setVec3(buffer, pointLightColors[i]);
        sprintf_s(buffer, "pointLights[%i].specular", i);
        ourShader.setVec3(buffer, 1.0f,1.0f,1.0f);

    }

    ourShader.setFloat("material.shininess", 6.0f);


    //custom hardcoded dataset
    glm::vec3 heightData[] = {
        glm::vec3(0,0,0.011009967885911465),
        glm::vec3(1,0,-0.02742447517812252),
        glm::vec3(2,0,-0.060354944318532944),
        glm::vec3(3,0,-0.07476402074098587),
        glm::vec3(4,0,-0.06774428486824036),
        glm::vec3(5,0,-0.04532375931739807),
        glm::vec3(6,0,-0.01719268597662449),
        glm::vec3(7,0,0.008326471783220768),
        glm::vec3(8,0,0.026501379907131195),
        glm::vec3(9,0,0.03568962216377258),
        glm::vec3(10,0,0.03568962216377258),
        glm::vec3(11,0,0.026501379907131195),
        glm::vec3(12,0,0.008326471783220768),
        glm::vec3(13,0,-0.01719268597662449),
        glm::vec3(14,0,-0.04532375931739807),
        glm::vec3(15,0,-0.06774428486824036),
        glm::vec3(16,0,-0.07476402074098587),
        glm::vec3(17,0,-0.060354944318532944),
        glm::vec3(18,0,-0.02742447517812252),
        glm::vec3(19,0,0.011009967885911465),
        glm::vec3(0,1,-0.02742447517812252),
        glm::vec3(1,1,-0.06354810297489166),
        glm::vec3(2,1,-0.07471580803394318),
        glm::vec3(3,1,-0.056113310158252716),
        glm::vec3(4,1,-0.01719268597662449),
        glm::vec3(5,1,0.026501379907131195),
        glm::vec3(6,1,0.06246454641222954),
        glm::vec3(7,1,0.08566688001155853),
        glm::vec3(8,1,0.09749827533960342),
        glm::vec3(9,1,0.10193675011396408),
        glm::vec3(10,1,0.10193675011396408),
        glm::vec3(11,1,0.09749827533960342),
        glm::vec3(12,1,0.08566688001155853),
        glm::vec3(13,1,0.06246454641222954),
        glm::vec3(14,1,0.026501379907131195),
        glm::vec3(15,1,-0.01719268597662449),
        glm::vec3(16,1,-0.056113310158252716),
        glm::vec3(17,1,-0.07471580803394318),
        glm::vec3(18,1,-0.06354810297489166),
        glm::vec3(19,1,-0.02742447517812252),
        glm::vec3(0,2,-0.060354944318532944),
        glm::vec3(1,2,-0.07471580803394318),
        glm::vec3(2,2,-0.051014043390750885),
        glm::vec3(3,2,-0.00048700900515541434),
        glm::vec3(4,2,0.05377919599413872),
        glm::vec3(5,2,0.09203202277421951),
        glm::vec3(6,2,0.10722985118627548),
        glm::vec3(7,2,0.10450868308544159),
        glm::vec3(8,2,0.0945383831858635),
        glm::vec3(9,2,0.08694256097078323),
        glm::vec3(10,2,0.08694256097078323),
        glm::vec3(11,2,0.0945383831858635),
        glm::vec3(12,2,0.10450868308544159),
        glm::vec3(13,2,0.10722985118627548),
        glm::vec3(14,2,0.09203202277421951),
        glm::vec3(15,2,0.05377919599413872),
        glm::vec3(16,2,-0.00048700900515541434),
        glm::vec3(17,2,-0.051014043390750885),
        glm::vec3(18,2,-0.07471580803394318),
        glm::vec3(19,2,-0.060354944318532944),
        glm::vec3(0,3,-0.07476402074098587),
        glm::vec3(1,3,-0.056113310158252716),
        glm::vec3(2,3,-0.00048700900515541434),
        glm::vec3(3,3,0.06246446818113327),
        glm::vec3(4,3,0.10193673521280289),
        glm::vec3(5,3,0.10450868308544159),
        glm::vec3(6,3,0.07759036123752594),
        glm::vec3(7,3,0.03945251554250717),
        glm::vec3(8,3,0.006805097218602896),
        glm::vec3(9,3,-0.011074644513428211),
        glm::vec3(10,3,-0.011074644513428211),
        glm::vec3(11,3,0.006805097218602896),
        glm::vec3(12,3,0.03945251554250717),
        glm::vec3(13,3,0.07759036123752594),
        glm::vec3(14,3,0.10450868308544159),
        glm::vec3(15,3,0.10193673521280289),
        glm::vec3(16,3,0.06246446818113327),
        glm::vec3(17,3,-0.00048700900515541434),
        glm::vec3(18,3,-0.056113310158252716),
        glm::vec3(19,3,-0.07476402074098587),
        glm::vec3(0,4,-0.06774428486824036),
        glm::vec3(1,4,-0.01719268597662449),
        glm::vec3(2,4,0.05377919599413872),
        glm::vec3(3,4,0.10193673521280289),
        glm::vec3(4,4,0.10038286447525024),
        glm::vec3(5,4,0.05376124754548073),
        glm::vec3(6,4,-0.011074644513428211),
        glm::vec3(7,4,-0.06692691147327423),
        glm::vec3(8,4,-0.10133937746286392),
        glm::vec3(9,4,-0.11593545973300934),
        glm::vec3(10,4,-0.11593545973300934),
        glm::vec3(11,4,-0.10133937746286392),
        glm::vec3(12,4,-0.06692691147327423),
        glm::vec3(13,4,-0.011074644513428211),
        glm::vec3(14,4,0.05376124754548073),
        glm::vec3(15,4,0.10038286447525024),
        glm::vec3(16,4,0.10193673521280289),
        glm::vec3(17,4,0.05377919599413872),
        glm::vec3(18,4,-0.01719268597662449),
        glm::vec3(19,4,-0.06774428486824036),
        glm::vec3(0,5,-0.04532375931739807),
        glm::vec3(1,5,0.026501379907131195),
        glm::vec3(2,5,0.09203202277421951),
        glm::vec3(3,5,0.10450868308544159),
        glm::vec3(4,5,0.05376124754548073),
        glm::vec3(5,5,-0.029588690027594566),
        glm::vec3(6,5,-0.10133934020996094),
        glm::vec3(7,5,-0.13631106913089752),
        glm::vec3(8,5,-0.13957053422927856),
        glm::vec3(9,5,-0.13270995020866394),
        glm::vec3(10,5,-0.13270995020866394),
        glm::vec3(11,5,-0.13957053422927856),
        glm::vec3(12,5,-0.13631106913089752),
        glm::vec3(13,5,-0.10133934020996094),
        glm::vec3(14,5,-0.029588690027594566),
        glm::vec3(15,5,0.05376124754548073),
        glm::vec3(16,5,0.10450868308544159),
        glm::vec3(17,5,0.09203202277421951),
        glm::vec3(18,5,0.026501379907131195),
        glm::vec3(19,5,-0.04532375931739807),
        glm::vec3(0,6,-0.01719268597662449),
        glm::vec3(1,6,0.06246454641222954),
        glm::vec3(2,6,0.10722985118627548),
        glm::vec3(3,6,0.07759036123752594),
        glm::vec3(4,6,-0.011074644513428211),
        glm::vec3(5,6,-0.10133934020996094),
        glm::vec3(6,6,-0.14049646258354187),
        glm::vec3(7,6,-0.11915365606546402),
        glm::vec3(8,6,-0.06968702375888824),
        glm::vec3(9,6,-0.03340946510434151),
        glm::vec3(10,6,-0.03340946510434151),
        glm::vec3(11,6,-0.06968702375888824),
        glm::vec3(12,6,-0.11915365606546402),
        glm::vec3(13,6,-0.14049646258354187),
        glm::vec3(14,6,-0.10133934020996094),
        glm::vec3(15,6,-0.011074644513428211),
        glm::vec3(16,6,0.07759036123752594),
        glm::vec3(17,6,0.10722985118627548),
        glm::vec3(18,6,0.06246454641222954),
        glm::vec3(19,6,-0.01719268597662449),
        glm::vec3(0,7,0.008326471783220768),
        glm::vec3(1,7,0.08566688001155853),
        glm::vec3(2,7,0.10450868308544159),
        glm::vec3(3,7,0.03945251554250717),
        glm::vec3(4,7,-0.06692691147327423),
        glm::vec3(5,7,-0.13631106913089752),
        glm::vec3(6,7,-0.11915365606546402),
        glm::vec3(7,7,-0.03340949863195419),
        glm::vec3(8,7,0.05837426707148552),
        glm::vec3(9,7,0.10836448520421982),
        glm::vec3(10,7,0.10836448520421982),
        glm::vec3(11,7,0.05837426707148552),
        glm::vec3(12,7,-0.03340949863195419),
        glm::vec3(13,7,-0.11915365606546402),
        glm::vec3(14,7,-0.13631106913089752),
        glm::vec3(15,7,-0.06692691147327423),
        glm::vec3(16,7,0.03945251554250717),
        glm::vec3(17,7,0.10450868308544159),
        glm::vec3(18,7,0.08566688001155853),
        glm::vec3(19,7,0.008326471783220768),
        glm::vec3(0,8,0.026501379907131195),
        glm::vec3(1,8,0.09749827533960342),
        glm::vec3(2,8,0.0945383831858635),
        glm::vec3(3,8,0.006805097218602896),
        glm::vec3(4,8,-0.10133937746286392),
        glm::vec3(5,8,-0.13957053422927856),
        glm::vec3(6,8,-0.06968702375888824),
        glm::vec3(7,8,0.05837426707148552),
        glm::vec3(8,8,0.15236231684684753),
        glm::vec3(9,8,0.17378069460391998),
        glm::vec3(10,8,0.17378069460391998),
        glm::vec3(11,8,0.15236231684684753),
        glm::vec3(12,8,0.05837426707148552),
        glm::vec3(13,8,-0.06968702375888824),
        glm::vec3(14,8,-0.13957053422927856),
        glm::vec3(15,8,-0.10133937746286392),
        glm::vec3(16,8,0.006805097218602896),
        glm::vec3(17,8,0.0945383831858635),
        glm::vec3(18,8,0.09749827533960342),
        glm::vec3(19,8,0.026501379907131195),
        glm::vec3(0,9,0.03568962216377258),
        glm::vec3(1,9,0.10193675011396408),
        glm::vec3(2,9,0.08694256097078323),
        glm::vec3(3,9,-0.011074644513428211),
        glm::vec3(4,9,-0.11593545973300934),
        glm::vec3(5,9,-0.13270995020866394),
        glm::vec3(6,9,-0.03340946510434151),
        glm::vec3(7,9,0.10836448520421982),
        glm::vec3(8,9,0.17378069460391998),
        glm::vec3(9,9,0.23044022917747498),
        glm::vec3(10,9,0.23044022917747498),
        glm::vec3(11,9,0.17378069460391998),
        glm::vec3(12,9,0.10836448520421982),
        glm::vec3(13,9,-0.03340946510434151),
        glm::vec3(14,9,-0.13270995020866394),
        glm::vec3(15,9,-0.11593545973300934),
        glm::vec3(16,9,-0.011074644513428211),
        glm::vec3(17,9,0.08694256097078323),
        glm::vec3(18,9,0.10193675011396408),
        glm::vec3(19,9,0.03568962216377258),
        glm::vec3(0,10,0.03568962216377258),
        glm::vec3(1,10,0.10193675011396408),
        glm::vec3(2,10,0.08694256097078323),
        glm::vec3(3,10,-0.011074644513428211),
        glm::vec3(4,10,-0.11593545973300934),
        glm::vec3(5,10,-0.13270995020866394),
        glm::vec3(6,10,-0.03340946510434151),
        glm::vec3(7,10,0.10836448520421982),
        glm::vec3(8,10,0.17378069460391998),
        glm::vec3(9,10,0.23044022917747498),
        glm::vec3(10,10,0.23044022917747498),
        glm::vec3(11,10,0.17378069460391998),
        glm::vec3(12,10,0.10836448520421982),
        glm::vec3(13,10,-0.03340946510434151),
        glm::vec3(14,10,-0.13270995020866394),
        glm::vec3(15,10,-0.11593545973300934),
        glm::vec3(16,10,-0.011074644513428211),
        glm::vec3(17,10,0.08694256097078323),
        glm::vec3(18,10,0.10193675011396408),
        glm::vec3(19,10,0.03568962216377258),
        glm::vec3(0,11,0.026501379907131195),
        glm::vec3(1,11,0.09749827533960342),
        glm::vec3(2,11,0.0945383831858635),
        glm::vec3(3,11,0.006805097218602896),
        glm::vec3(4,11,-0.10133937746286392),
        glm::vec3(5,11,-0.13957053422927856),
        glm::vec3(6,11,-0.06968702375888824),
        glm::vec3(7,11,0.05837426707148552),
        glm::vec3(8,11,0.15236231684684753),
        glm::vec3(9,11,0.17378069460391998),
        glm::vec3(10,11,0.17378069460391998),
        glm::vec3(11,11,0.15236231684684753),
        glm::vec3(12,11,0.05837426707148552),
        glm::vec3(13,11,-0.06968702375888824),
        glm::vec3(14,11,-0.13957053422927856),
        glm::vec3(15,11,-0.10133937746286392),
        glm::vec3(16,11,0.006805097218602896),
        glm::vec3(17,11,0.0945383831858635),
        glm::vec3(18,11,0.09749827533960342),
        glm::vec3(19,11,0.026501379907131195),
        glm::vec3(0,12,0.008326471783220768),
        glm::vec3(1,12,0.08566688001155853),
        glm::vec3(2,12,0.10450868308544159),
        glm::vec3(3,12,0.03945251554250717),
        glm::vec3(4,12,-0.06692691147327423),
        glm::vec3(5,12,-0.13631106913089752),
        glm::vec3(6,12,-0.11915365606546402),
        glm::vec3(7,12,-0.03340949863195419),
        glm::vec3(8,12,0.05837426707148552),
        glm::vec3(9,12,0.10836448520421982),
        glm::vec3(10,12,0.10836448520421982),
        glm::vec3(11,12,0.05837426707148552),
        glm::vec3(12,12,-0.03340949863195419),
        glm::vec3(13,12,-0.11915365606546402),
        glm::vec3(14,12,-0.13631106913089752),
        glm::vec3(15,12,-0.06692691147327423),
        glm::vec3(16,12,0.03945251554250717),
        glm::vec3(17,12,0.10450868308544159),
        glm::vec3(18,12,0.08566688001155853),
        glm::vec3(19,12,0.008326471783220768),
        glm::vec3(0,13,-0.01719268597662449),
        glm::vec3(1,13,0.06246454641222954),
        glm::vec3(2,13,0.10722985118627548),
        glm::vec3(3,13,0.07759036123752594),
        glm::vec3(4,13,-0.011074644513428211),
        glm::vec3(5,13,-0.10133934020996094),
        glm::vec3(6,13,-0.14049646258354187),
        glm::vec3(7,13,-0.11915365606546402),
        glm::vec3(8,13,-0.06968702375888824),
        glm::vec3(9,13,-0.03340946510434151),
        glm::vec3(10,13,-0.03340946510434151),
        glm::vec3(11,13,-0.06968702375888824),
        glm::vec3(12,13,-0.11915365606546402),
        glm::vec3(13,13,-0.14049646258354187),
        glm::vec3(14,13,-0.10133934020996094),
        glm::vec3(15,13,-0.011074644513428211),
        glm::vec3(16,13,0.07759036123752594),
        glm::vec3(17,13,0.10722985118627548),
        glm::vec3(18,13,0.06246454641222954),
        glm::vec3(19,13,-0.01719268597662449),
        glm::vec3(0,14,-0.04532375931739807),
        glm::vec3(1,14,0.026501379907131195),
        glm::vec3(2,14,0.09203202277421951),
        glm::vec3(3,14,0.10450868308544159),
        glm::vec3(4,14,0.05376124754548073),
        glm::vec3(5,14,-0.029588690027594566),
        glm::vec3(6,14,-0.10133934020996094),
        glm::vec3(7,14,-0.13631106913089752),
        glm::vec3(8,14,-0.13957053422927856),
        glm::vec3(9,14,-0.13270995020866394),
        glm::vec3(10,14,-0.13270995020866394),
        glm::vec3(11,14,-0.13957053422927856),
        glm::vec3(12,14,-0.13631106913089752),
        glm::vec3(13,14,-0.10133934020996094),
        glm::vec3(14,14,-0.029588690027594566),
        glm::vec3(15,14,0.05376124754548073),
        glm::vec3(16,14,0.10450868308544159),
        glm::vec3(17,14,0.09203202277421951),
        glm::vec3(18,14,0.026501379907131195),
        glm::vec3(19,14,-0.04532375931739807),
        glm::vec3(0,15,-0.06774428486824036),
        glm::vec3(1,15,-0.01719268597662449),
        glm::vec3(2,15,0.05377919599413872),
        glm::vec3(3,15,0.10193673521280289),
        glm::vec3(4,15,0.10038286447525024),
        glm::vec3(5,15,0.05376124754548073),
        glm::vec3(6,15,-0.011074644513428211),
        glm::vec3(7,15,-0.06692691147327423),
        glm::vec3(8,15,-0.10133937746286392),
        glm::vec3(9,15,-0.11593545973300934),
        glm::vec3(10,15,-0.11593545973300934),
        glm::vec3(11,15,-0.10133937746286392),
        glm::vec3(12,15,-0.06692691147327423),
        glm::vec3(13,15,-0.011074644513428211),
        glm::vec3(14,15,0.05376124754548073),
        glm::vec3(15,15,0.10038286447525024),
        glm::vec3(16,15,0.10193673521280289),
        glm::vec3(17,15,0.05377919599413872),
        glm::vec3(18,15,-0.01719268597662449),
        glm::vec3(19,15,-0.06774428486824036),
        glm::vec3(0,16,-0.07476402074098587),
        glm::vec3(1,16,-0.056113310158252716),
        glm::vec3(2,16,-0.00048700900515541434),
        glm::vec3(3,16,0.06246446818113327),
        glm::vec3(4,16,0.10193673521280289),
        glm::vec3(5,16,0.10450868308544159),
        glm::vec3(6,16,0.07759036123752594),
        glm::vec3(7,16,0.03945251554250717),
        glm::vec3(8,16,0.006805097218602896),
        glm::vec3(9,16,-0.011074644513428211),
        glm::vec3(10,16,-0.011074644513428211),
        glm::vec3(11,16,0.006805097218602896),
        glm::vec3(12,16,0.03945251554250717),
        glm::vec3(13,16,0.07759036123752594),
        glm::vec3(14,16,0.10450868308544159),
        glm::vec3(15,16,0.10193673521280289),
        glm::vec3(16,16,0.06246446818113327),
        glm::vec3(17,16,-0.00048700900515541434),
        glm::vec3(18,16,-0.056113310158252716),
        glm::vec3(19,16,-0.07476402074098587),
        glm::vec3(0,17,-0.060354944318532944),
        glm::vec3(1,17,-0.07471580803394318),
        glm::vec3(2,17,-0.051014043390750885),
        glm::vec3(3,17,-0.00048700900515541434),
        glm::vec3(4,17,0.05377919599413872),
        glm::vec3(5,17,0.09203202277421951),
        glm::vec3(6,17,0.10722985118627548),
        glm::vec3(7,17,0.10450868308544159),
        glm::vec3(8,17,0.0945383831858635),
        glm::vec3(9,17,0.08694256097078323),
        glm::vec3(10,17,0.08694256097078323),
        glm::vec3(11,17,0.0945383831858635),
        glm::vec3(12,17,0.10450868308544159),
        glm::vec3(13,17,0.10722985118627548),
        glm::vec3(14,17,0.09203202277421951),
        glm::vec3(15,17,0.05377919599413872),
        glm::vec3(16,17,-0.00048700900515541434),
        glm::vec3(17,17,-0.051014043390750885),
        glm::vec3(18,17,-0.07471580803394318),
        glm::vec3(19,17,-0.060354944318532944),
        glm::vec3(0,18,-0.02742447517812252),
        glm::vec3(1,18,-0.06354810297489166),
        glm::vec3(2,18,-0.07471580803394318),
        glm::vec3(3,18,-0.056113310158252716),
        glm::vec3(4,18,-0.01719268597662449),
        glm::vec3(5,18,0.026501379907131195),
        glm::vec3(6,18,0.06246454641222954),
        glm::vec3(7,18,0.08566688001155853),
        glm::vec3(8,18,0.09749827533960342),
        glm::vec3(9,18,0.10193675011396408),
        glm::vec3(10,18,0.10193675011396408),
        glm::vec3(11,18,0.09749827533960342),
        glm::vec3(12,18,0.08566688001155853),
        glm::vec3(13,18,0.06246454641222954),
        glm::vec3(14,18,0.026501379907131195),
        glm::vec3(15,18,-0.01719268597662449),
        glm::vec3(16,18,-0.056113310158252716),
        glm::vec3(17,18,-0.07471580803394318),
        glm::vec3(18,18,-0.06354810297489166),
        glm::vec3(19,18,-0.02742447517812252),
        glm::vec3(0,19,0.011009967885911465),
        glm::vec3(1,19,-0.02742447517812252),
        glm::vec3(2,19,-0.060354944318532944),
        glm::vec3(3,19,-0.07476402074098587),
        glm::vec3(4,19,-0.06774428486824036),
        glm::vec3(5,19,-0.04532375931739807),
        glm::vec3(6,19,-0.01719268597662449),
        glm::vec3(7,19,0.008326471783220768),
        glm::vec3(8,19,0.026501379907131195),
        glm::vec3(9,19,0.03568962216377258),
        glm::vec3(10,19,0.03568962216377258),
        glm::vec3(11,19,0.026501379907131195),
        glm::vec3(12,19,0.008326471783220768),
        glm::vec3(13,19,-0.01719268597662449),
        glm::vec3(14,19,-0.04532375931739807),
        glm::vec3(15,19,-0.06774428486824036),
        glm::vec3(16,19,-0.07476402074098587),
        glm::vec3(17,19,-0.060354944318532944),
        glm::vec3(18,19,-0.02742447517812252),
        glm::vec3(19,19,0.011009967885911465)
    };


    
    for (int i = 0; i < 400; i++) {
        char buffer[64];
        sprintf_s(buffer, "data[%i]", i);
        ourShader.setVec3(buffer, heightData[i]);
    }
   

    //create grid
    std::vector<glm::vec3> vertices;
    std::vector <glm::uvec3> indices;
    std::vector<glm::vec3> normals;
    genGrid(19, vertices, indices);

    GLuint vao = generate_vao(vertices, indices);

    ourShader.setVec3("vertexColor", glm::vec3(0.5, 0.2, 0.2));
    ourShader.setVec2("highlights", glm::vec2(curRow, curColumn));

    
    
    //enable depth test

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    // stencil testing

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


   //enable culling

    //glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);


    //wireframe

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // gl buffer object -----------------------------------
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2* sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    //set projection matrix once (no zoom functionality) 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
   

    //60fps = 16.67 ms

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //fps counters
        std::chrono::high_resolution_clock timer;
        auto start = timer.now();


        //input
        ProcessInput(window); 

        //first Pass-----------------------------------------------------------------

        
        glEnable(GL_DEPTH_TEST);
        glClearColor(ambientCol.x,ambientCol.y,ambientCol.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        


        // view transformations
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


       
       
        // don't forget to enable shader before setting uniforms
        ourShader.use();


        // place model 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));	
        //set model
        ourShader.setMat4("model", model);
        


        //live Data example

        
        for (int i = 0; i < 400; i++) {

            char buffer[64];
            sprintf_s(buffer, "data[%i]", i);

            if (liveData) 
            {
                glm::vec3 liveData = heightData[i];

                //create a sine wave originating from the center
                float lenCenter = glm::length(glm::vec2(liveData.x -10, liveData.y-10));
                liveData.z = sin(lastFrame * 0.5f + lenCenter * 0.6f) * 0.2f;
                
                
                ourShader.setVec3(buffer, liveData);    
            }
            else{
                ourShader.setVec3(buffer, heightData[i]);
            }


        }
        
       

        //set highlights
        ourShader.setBool("shading", shading);
        ourShader.setVec2("highlights", glm::vec2(curRow, curColumn));


       
        draw_vao(vao, (GLsizei)indices.size() * 3);


        glfwSwapBuffers(window);
        glfwPollEvents();



        //frame limiting
        auto stop = timer.now();
        auto delta = stop - start;
        auto totMs = std::chrono::milliseconds(8);
        std::this_thread::sleep_for(totMs - delta);
        auto secondStop = timer.now();
        typedef std::chrono::milliseconds ms;
        typedef std::chrono::duration<float> fsec;
        fsec fs =  secondStop - start;
        ms d = std::chrono::duration_cast<ms>(fs);
        

        
    }

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

  
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS) shading = !shading;
    if (key == GLFW_KEY_L && action == GLFW_PRESS) liveData = !liveData;

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) curColumn += 1;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) curColumn -= 1;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) curRow += 1;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) curRow -= 1;

    curRow = glm::max(0, glm::min(19, curRow));
    curColumn = glm::max(0, glm::min(19, curColumn));
}

void mouse_callback(GLFWwindow*, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);

   
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void genGrid(int N, std::vector<glm::vec3>  &vertices, std::vector<glm::uvec3> &indices)
{
    for (int j = 0; j <= N; j++)
    {
        for (int i = 0; i <= N; i++)
        {
            float x = (float)i / float(N);
            float y = (float)j / float(N);
            float z = 0;
            vertices.push_back(glm::vec3(x, y, z));

        }
    }
    for (int j = 0; j < N; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            int row1 = j * (N + 1);
            int row2 = (j + 1) * (N + 1);

            // triangle 1
            indices.push_back(glm::uvec3(row1 + i, row1 + i + 1, row2 + i + 1));

            // triangle 2
            indices.push_back(glm::uvec3(row1 + i, row2 + i + 1, row2 + i));
        }
    }
}

GLuint generate_vao(const std::vector<glm::vec3>& vertices, const std::vector<glm::uvec3>& indices)
{
    GLuint  vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}


void draw_vao(GLuint vao, GLsizei n)
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)n, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}







