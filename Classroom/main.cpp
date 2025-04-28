#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"


#include <iostream>
#include <cmath>

//#define PI 3.14159265358979323846

using namespace std;





void perspectiveProjection(float fov, float aspect, float near, float far, float matrix[16]) {
    float tanHalfFov = tanf(fov / 2.0f);

    
    for (int i = 0; i < 16; ++i)
        matrix[i] = 0.0f;

    matrix[0] = 1.0f / (aspect * tanHalfFov);  // (1 / (aspect * tan(fov / 2)))
    matrix[5] = 1.0f / tanHalfFov;  // (1 / tan(fov / 2))
    matrix[10] = -(far + near) / (far - near);  // (-(far + near) / (far - near))
    matrix[11] = -1.0f;  // -1
    matrix[14] = -(2 * far * near) / (far - near);  // (-(2 * far * near) / (far - near))
    matrix[15] = 0.0f;

    
    matrix[1] = matrix[2] = matrix[3] = 0.0f;
    matrix[4] = matrix[6] = matrix[7] = 0.0f;
    matrix[8] = matrix[9] = matrix[12] = matrix[13] = matrix[15] = 0.0f;
}




void orthogonalProjection(float left, float right, float bottom, float top, float near, float far, float matrix[16]) {
    
    float r_l = right - left;
    float t_b = top - bottom;
    float n_f = far - near;

    
    for (int i = 0; i < 16; ++i)
        matrix[i] = 0.0f;

    
    matrix[0] = 2.0f / r_l;  // (2 / (right - left))
    matrix[5] = 2.0f / t_b;  // (2 / (top - bottom))
    matrix[10] = 2.0f / n_f; // (2 / (near - far))
    matrix[12] = (right + left) / r_l;   // (right + left) / (right - left)
    matrix[13] = (top + bottom) / t_b;   // (top + bottom) / (top - bottom)
    matrix[14] = (far + near) / n_f;     // (far + near) / (near - far)
    matrix[15] = 1.0f;

    
    matrix[1] = matrix[2] = matrix[3] = 0.0f;
    matrix[4] = matrix[6] = matrix[7] = 0.0f;
    matrix[8] = matrix[9] = matrix[11] = 0.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void draw(unsigned int VAO, glm::mat4 sm);
void rotate_fan(unsigned int VAO, glm::mat4 sm);
void bed(unsigned int& VAO, Shader& lightingShader, glm::mat4 alTogether);
void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model , float r , float g , float b );
void drawBoard(unsigned int& cVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void board(unsigned int& cVAO, Shader& lightingShader, glm::mat4 alTogether);
void drawWall(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void draw_tablechair(unsigned int& cVAO, Shader& lightingShader, glm::mat4 sm);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transformxxccdrrrfffgghgge
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 0.5;

float r = 0.0;

bool fanOn = false;
glm::vec3 cam = glm::vec3(-2.0f, 0.5f, -.5f);


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//float eyeX = 0.0, eyeY = 0.0, eyeZ = 3.0;
float eyeX = 2.0, eyeY = 1.0, eyeZ = -11.0;
float lookAtX = -2.9, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);



// positions of the point lights
glm::vec3 pointLightPositions[] = {
     glm::vec3(1.0f,  3.3f,  2.5f),
    glm::vec3(-9.50f,  3.4f,  2.5f),
    //glm::vec3(1.5f,  -1.5f,  0.0f),
   
    //-0.7f, 0.0f, 5.8f
    //glm::vec3(-1.5f,  -1.5f,  0.0f)
};


//glm::vec3(-0.5, 1, -0.5)


PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    1.0f, 1.0f, 1.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
//PointLight pointlight3(
//
//    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
//    0.05f, 0.05f, 0.05f,     // ambient
//    1.0f, 1.0f, 1.0f,     // diffuse
//    1.0f, 1.0f, 1.0f,        // specular
//    1.0f,   //k_c
//    0.09f,  //k_l
//    0.032f, //k_q
//    3       // light number
//);
//PointLight pointlight4(
//
//    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
//    0.05f, 0.05f, 0.05f,     // ambient
//    1.0f, 1.0f, 1.0f,     // diffuse
//    1.0f, 1.0f, 1.0f,        // specular
//    1.0f,   //k_c
//    0.09f,  //k_l
//    0.032f, //k_q
//    4       // light number
//);


// light settings
bool onOffToggle = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;

bool point1Toggle = true;
bool point2Toggle = true;
bool directionToggle = true;
bool spotToggle = true;



// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes

    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f,
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);



    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        //pointlight3.setUpPointLight(lightingShader);
        // point light 4
        //pointlight4.setUpPointLight(lightingShader);

        lightingShader.setVec3("diectionalLight.directiaon", 0.0f, 0.0f, -3.0f);
        lightingShader.setVec3("diectionalLight.ambient", .2, .2, .2);
        lightingShader.setVec3("diectionalLight.diffuse", .8f, .8f, .8f);
        lightingShader.setVec3("diectionalLight.specular", 1.0f, 1.0f, 1.0f);


        if (directionToggle)
        {

            lightingShader.setBool("dlighton", true);
        }
        else if(!directionToggle)
        {
            lightingShader.setBool("dlighton", false);
        }

        


        lightingShader.setVec3("spotlight.position", -0.5, 1, -0.5);
        lightingShader.setVec3("spotlight.direction", 0, -1, 0);
        lightingShader.setVec3("spotlight.ambient", .2, .2, .2);
        lightingShader.setVec3("spotlight.diffuse", .8f, .8f, .8f);
        lightingShader.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotlight.k_c", 1.0f);
        lightingShader.setFloat("spotlight.k_l", 0.09);
        lightingShader.setFloat("spotlight.k_q", 0.032);
        lightingShader.setFloat("cos_theta", glm::cos(glm::radians(5.5f)));


        if (spotToggle)
        {

            lightingShader.setBool("spotlighton", true);

        }
        else if (!spotToggle)
        {
            lightingShader.setBool("spotlighton", false);

        }

        

        //pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
        //    1.0f, 1.0f, 1.0f,     // ambient
        //    1.0f, 1.0f, 1.0f,      // diffuse
        //    1.0f, 1.0f, 1.0f,        // specular
        //    1.0f,   //k_c
        //    0.09f,  //k_l
        //    0.032f, //k_q
        //    1,       // light number
        //    glm::cos(glm::radians(20.5f)),
        //    glm::cos(glm::radians(25.0f)),
        //    0, -1, 0
        // activate shader
        lightingShader.use();





        float cameraZoom = camera.Zoom;  // Example camera zoom (adjust as needed)
        float SCR_WIDTH = 800.0f;  // Screen width
        float SCR_HEIGHT = 600.0f;  // Screen height
        float near = 0.1f;  // Near clipping plane
        float far = 100.0f;  // Far clipping plane

        
        float fov = glm::radians(cameraZoom);

        // Aspect ratio (width / height)
        float aspect = SCR_WIDTH / SCR_HEIGHT;

        // Create a matrix to store the result
        float projectionMatrix[16];

        // Calculate the perspective matrix manually
        perspectiveProjection(fov, aspect, near, far, projectionMatrix);

        // Get the uniform location for the 'projection' matrix in your shader
        GLint projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");

        // Send the orthogonal projection matrix to the shader
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projectionMatrix);
        // pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        //lightingShader.setMat4("projection", projection);

        // camera/view transformation
        //glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); 
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        //bed(VAO, lightingShader, modell);
        board(VAO, lightingShader, model);

        // also draw the lamp object(s)
        ourShader.use();
        //ourShader.setMat4("projection", projection);
        GLint projectionLoc1 = glGetUniformLocation(ourShader.ID, "projection");

        // Send the orthogonal projection matrix to the shader
        glUniformMatrix4fv(projectionLoc1, 1, GL_FALSE, projectionMatrix);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.4f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f,1.0f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //// activate shader
        //ourShader.use();

        //// pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //ourShader.setMat4("projection", projection);

        ////glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();

        //ourShader.setMat4("view", view);


        //glm::mat4 identityMatrix = glm::mat4(1.0f);
        //glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;

        //ground
        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, -0.001f, -22.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)); // Medium gray



        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //back-side
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, 11.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.6, 0.4, 0.4, 1.0)); // Slightly dark off-white



        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 4.5f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, -0.001f, -22.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));


        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //sidelwalls
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 11.0f, -22.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.5f, -1.0f, 3.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 11.0f, -22.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Almirah
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, -1.0f, -0.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 7.0f, -4.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.3f, 1.0f)); // Dark Navy Blue

        //ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        */


        // Black Board


        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.2f, 0.0f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-12.0f, 5.0f, 0.2f));
        model = translateMatrix * scaleMatrix;

        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


        translateMatrix = identityMatrix;
        rotateXMatrix = identityMatrix;
        //draw(VAO, identityMatrix);
        draw_tablechair(VAO, lightingShader, identityMatrix);

        for (int i = 0; i < 3; i++)
        {
            translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 0.0f));
            /*rotateXMatrix = rotateXMatrix*glm::rotate(identityMatrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateXMatrix = rotateXMatrix * translateMatrix;*/
            //draw(VAO, translateMatrix);
            draw_tablechair(VAO, lightingShader, translateMatrix);
        }
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
        //draw(VAO, translateMatrix);
        draw_tablechair(VAO, lightingShader, translateMatrix);

        for (int i = 0; i < 3; i++)
        {
            translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 0.0f));

            
            ///draw(VAO, translateMatrix);
            draw_tablechair(VAO, lightingShader, translateMatrix);
        }

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -4.0f));
        //draw(VAO, translateMatrix);
        draw_tablechair(VAO, lightingShader, translateMatrix);
        for (int i = 0; i < 3; i++)
        {
            translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 0.0f));

            //draw(VAO, translateMatrix);
            draw_tablechair(VAO, lightingShader, translateMatrix);

        }

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -6.0f));
        //draw(VAO, translateMatrix);
        draw_tablechair(VAO, lightingShader, translateMatrix);

        for (int i = 0; i < 3; i++)
        {
            translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, 0.0f));

            //draw(VAO, translateMatrix);
            draw_tablechair(VAO, lightingShader, translateMatrix);

        }


        //FAN
        if (fanOn)
        {
            translateMatrix = identityMatrix;

            rotate_fan(VAO, translateMatrix);




            r = (r + 5.0);

        }
        else
        {
            translateMatrix = identityMatrix;
            rotate_fan(VAO, identityMatrix);

        }




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, 0.0, 0.0));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, 0.0, 0.0));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    lightingShader.setVec3("material.emission", glm::vec3(0.5f, 0.0f, 0.0f));

    lightingShader.setMat4("model", model);

    glBindVertexArray(cVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void drawBoard(unsigned int& cVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 0.0f, float g = 0.0f, float b = 0.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, 0.0f, 0.0f));  // Black ambient color
    lightingShader.setVec3("material.diffuse", glm::vec3(r, 0.0f, 0.0f));  // Black diffuse color
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));  // Specular is kept red for contrast
    lightingShader.setFloat("material.shininess", 32.0f);  // Shininess of the material

    lightingShader.setVec3("material.emission", glm::vec3(0.2f, 0.2f, 0.2f));  // No emission (black emission)

    lightingShader.setMat4("model", model);

    glBindVertexArray(cVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void drawWall(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 0.0f, float g = 0.0f, float b = 0.0f)
{
    lightingShader.use();

    // Set the material properties for a blackboard (black color)
    lightingShader.setVec3("material.ambient", glm::vec3(r, 0.7 * g, 0.8)); // Set ambient to black (0, 0, 0)
    lightingShader.setVec3("material.diffuse", glm::vec3(r * 0.8, 0.9, b)); // Set diffuse to black
    lightingShader.setVec3("material.specular", glm::vec3(r, g, b)); // Set specular to black (no shine)
    lightingShader.setFloat("material.shininess", 1.0f); // Keep the shininess moderate
    //lightingShader.setVec3("material.emission", glm::vec3(0.9f, 0.3f, 0.3f)); 
    lightingShader.setVec3("material.emission", glm::vec3(0.3f, 0.3f, 0.3f));// No emission (blackboard is not glowing)

    // Set the model matrix to position/rotate the cube as needed
    lightingShader.setMat4("model", model);

    // Bind the cube's VAO and draw it
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void board(unsigned int& cVAO, Shader& lightingShader, glm::mat4 alTogether)
{

    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.7f, 0.0f, 5.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-12.0f, 5.0f, 0.2f));
    model = alTogether *  translateMatrix * scaleMatrix *  model;

    drawWall(cVAO, lightingShader, model, 0.0, 0.0, 0.0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, -0.001f, -22.0f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.1, 0.4, 0.9);
    //ourShader.setMat4("model", model);
   // ourShader.setVec4("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)); // Medium gray



    /*glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    //back-side
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, 11.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.8, 0.1, 0.4);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.6, 0.4, 0.4, 1.0)); // Slightly dark off-white



    /*glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    //roof
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 4.5f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-22.0f, -0.001f, -22.0f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
   /* ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    //sidelwalls
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, -1.0f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 11.0f, -22.0f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.8, 0.4, 0.9);
    /*ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.5f, -1.0f, 3.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 11.0f, -22.0f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.8, 0.4, 0.9);
    /*ourShader.setMat4("model", model);
    ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    //Almirah
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.0f, -1.0f, -0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-2.0f, 7.0f, -4.0f));
    model = translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.2, 0.2, 0.9);
    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.3f, 1.0f)); // Dark Navy Blue

    ////ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //ourShader.setMat4("model", model);
    //ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    //glBindVertexArray(cVAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    
}


void draw_tablechair(unsigned int& cVAO, Shader& lightingShader, glm::mat4 sm)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;


    //top of Table
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, 0.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, .2f, 2.0f));
    
    //model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, .8f, .6078f, 0.0f);
    /*ourShader.setMat4("model", model);
    * 
    ourShader.setVec4("color", glm::vec4(.7f, .2f, 0.0f, 1.0f));


    ourShader.setVec4("color", glm::vec4(.8f, .6078f, 0.0f, 1.0f));*/


    //left backside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, 0.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -2.0f, .2f));
    ////model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    //ourShader.setMat4("model", model);
    ////ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //left frontside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, -1.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    //ourShader.setMat4("model", model);
    ////ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right frontside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, -1.0f, 0.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    ////model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    //ourShader.setMat4("model", model);


    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right backside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(.15f, -1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    ////model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    //ourShader.setMat4("model", model);

    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Seat of Chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, .2f, 1.2f));

    model = sm * translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.2, 0.2, 0.2);
    /*ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(.3961f, .29411f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/

    //Back of chair
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.45f, -.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, .2f, .7f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = sm * translateMatrix * rotateXMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.898f, 0.894f, 0.0f);
    /*ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.898f, 0.894f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    // two back side of chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.4f, -.3f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, .4f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.0, 0.0, 0.0);
    /*ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.1490f, 0.1490f, 0.1490f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.2f, -.3f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, .4f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawWall(cVAO, lightingShader, model, 0.0, 0.0, 0.0);
    /*ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    //Leg of chair
    //ourShader.setVec4("color", glm::vec4(.3961f, .29411f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    /*ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.1f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    /*ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    //Front Leg of Chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, 0.4f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    /*ourShader.setMat4("model", model);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.1f, -.4f, 0.4f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    drawBoard(cVAO, lightingShader, model, 1.0, 0.4, 0.9);
    /* ourShader.setMat4("model", model);

     glBindVertexArray(VAO);
     glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);*/
}



void bed(unsigned int& cVAO, Shader& lightingShader, glm::mat4 alTogether)
{
    float baseHeight = 0.3;
    float width = 1;
    float length = 2;
    float pillowWidth = 0.3;
    float pillowLength = 0.15;
    float blanketWidth = 0.8;
    float blanketLength = 0.7;
    float headHeight = 0.6;

    //base
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(model, glm::vec3(width, baseHeight, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * scale * translate;
    drawCube(cVAO, lightingShader, model, 0.545, 0.271, 0.075);

    //foam
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight, 0));
    scale = glm::scale(model, glm::vec3(width, 0.06, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cVAO, lightingShader, model, 0.804, 0.361, 0.361);

    //pillow 1
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((width / 2) - (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cVAO, lightingShader, model, 1, 0.647, 0);

    //pillow 2
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((-width / 2) + (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cVAO, lightingShader, model, 1, 0.647, 0);

    //blanket
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight + 1 * 0.06, -(length / 2 - 0.025) + blanketLength / 2));
    scale = glm::scale(model, glm::vec3(blanketWidth, 0.015, blanketLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cVAO, lightingShader, model, 0.541, 0.169, 0.886);

    //head
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, 0, (length / 2 - 0.02 / 2) + 0.02));
    scale = glm::scale(model, glm::vec3(width, headHeight, 0.02));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cVAO, lightingShader, model, 0.545, 0.271, 0.075);

}


void rotate_fan(unsigned int VAO, glm::mat4 sm)
{
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    //OUTLINE WALL 
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 3.6f, 0.125f));

    glm::mat4 translateMatrix2 = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -0.125f));


    glm::mat4 translateMatrix3 = glm::translate(identityMatrix, glm::vec3(-1.5f, 0.0f, -1.7f));


    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, .5f));

    model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.0196, 0.0157f, 0.4f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 3.6f, 0.125f));

    translateMatrix2 = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -0.125f));


    model = translateMatrix3 * translateMatrix * glm::rotate(identityMatrix, glm::radians(r + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * translateMatrix2 * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.0196, 0.0157f, 0.4f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.1f, .6f, .1f));
    translateMatrix3 = glm::translate(identityMatrix, glm::vec3(-3.0f, 4.1f, -1.58f));

    model = translateMatrix3 * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.0, 0.0f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



}


void draw(unsigned int VAO, glm::mat4 sm)
{

    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;


    //top of Table
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, 0.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, .2f, 2.0f));
    //model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(.7f, .2f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    ourShader.setVec4("color", glm::vec4(.8f, .6078f, 0.0f, 1.0f));


    //left backside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, 0.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -2.0f, .2f));
    //model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    //ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //left frontside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.75f, -1.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    //ourShader.setVec3("aColor", glm::vec3(0.2f, 0.1f, 0.4f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right frontside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, -1.0f, 0.9f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    //model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right backside leg
    translateMatrix = glm::translate(identityMatrix, glm::vec3(.15f, -1.0f, 0.0f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, 2.0f, .2f));
    //model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Seat of Chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, .2f, 1.2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(.3961f, .29411f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Back of chair
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.45f, -.1f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, .2f, .7f));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = sm * translateMatrix * rotateXMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.898f, 0.894f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // two back side of chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.4f, -.3f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, .4f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.1490f, 0.1490f, 0.1490f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.2f, -.3f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, .4f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Leg of chair
    ourShader.setVec4("color", glm::vec4(.3961f, .29411f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.1f, -.4f, -0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Front Leg of Chair

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5f, -.4f, 0.4f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    translateMatrix = glm::translate(identityMatrix, glm::vec3(-.1f, -.4f, 0.4f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.2f, -1.2f, .2f));

    model = sm * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);




}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }*/
    /*if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 0.0f, -1.0f));
    }*/

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 1;
        else if (rotateAxis_Y) rotateAngle_Y -= 1;
        else rotateAngle_Z -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    //if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 1;
        rotateAxis_X = 1.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 0.0;

    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 1.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAxis_Z += 1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (fanOn)
            fanOn = false;
        else
            fanOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        lookAtX += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        lookAtX -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    /*if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        lookAtZ += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        lookAtZ -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
    }*/

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {

        if (directionToggle)
        {

            directionToggle = !directionToggle;
            
        }
        else
        {
            directionToggle = !directionToggle;
        }


    }

    //if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    //{
    //    pointlight1.turnOff();
    //    pointlight2.turnOff();
    //    //pointlight3.turnOff();
    //    //pointlight4.turnOff();

    //}
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (point1Toggle == true)
        {
            pointlight1.turnOff();
            point1Toggle = !point1Toggle;
        }
        else
        {
            pointlight1.turnOn();
            point1Toggle = !point1Toggle;
        }
        
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (point2Toggle == true)
        {
            pointlight2.turnOff();
            point2Toggle = !point1Toggle;
        }
        else
        {
            pointlight2.turnOn();
            point2Toggle = !point2Toggle;
        }

    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {

        if (spotToggle)
        {

            spotToggle = !spotToggle;

        }
        else
        {
            spotToggle = !spotToggle;
        }


    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        if (diffuseToggle)
        {

            pointlight1.turnDiffuseOff();
            pointlight2.turnDiffuseOff();
            //pointlight3.turnDiffuseOff();
            //pointlight4.turnDiffuseOff();
            diffuseToggle = !diffuseToggle;
        }
        else
        {

            pointlight1.turnDiffuseOn();
            pointlight2.turnDiffuseOn();
            //pointlight3.turnDiffuseOn();
            //pointlight4.turnDiffuseOn();
            diffuseToggle = !diffuseToggle;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        if (specularToggle)
        {

            pointlight1.turnSpecularOff();
            pointlight2.turnSpecularOff();
            //pointlight3.turnSpecularOff();
            //pointlight4.turnSpecularOff();
            specularToggle = !specularToggle;
        }
        else
        {

            pointlight1.turnSpecularOn();
            pointlight2.turnSpecularOn();
            //pointlight3.turnSpecularOn();
            //pointlight4.turnSpecularOn();
            specularToggle = !specularToggle;
        }



    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        

        if (ambientToggle)
        {

            pointlight1.turnAmbientOff();
            pointlight2.turnAmbientOff();
            //pointlight3.turnSpecularOff();
            //pointlight4.turnSpecularOff();
            ambientToggle = !ambientToggle;
        }
        else
        {

            pointlight1.turnAmbientOn();
            pointlight2.turnAmbientOn();
            //pointlight3.turnSpecularOn();
            //pointlight4.turnSpecularOn();
            ambientToggle = !ambientToggle;
        }


    }
    
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        pointlight1.ambient = glm::vec3(0.0, 1.0, 0.0);
        pointlight1.diffuse = glm::vec3(0.0, 1.0, 0.0);
        pointlight1.specular = glm::vec3(0.0, 1.0, 0.0);


        pointlight2.ambient = glm::vec3(0.0, 1.0, 0.0);
        pointlight2.diffuse = glm::vec3(0.0, 1.0, 0.0);
        pointlight2.specular = glm::vec3(0.0, 1.0, 0.0);


        //pointlight3.ambient = glm::vec3(0.0, 1.0, 0.0);
        //pointlight3.diffuse = glm::vec3(0.0, 1.0, 0.0);
        //pointlight3.specular = glm::vec3(0.0, 1.0, 0.0);


        //pointlight4.ambient = glm::vec3(0.0, 1.0, 0.0);
        //pointlight4.diffuse = glm::vec3(0.0, 1.0, 0.0);
        //pointlight4.specular = glm::vec3(0.0, 1.0, 0.0);

    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}