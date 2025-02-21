#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "bed.h"
#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "SpotLight.h"
#include "walls.h"
#include "bench.h"
#include "swing.h"
#include "stand.h"
#include "cube.h"
#include "stb_image.h"
#include "cylinderStand.h"
#include <iostream>
#include "sphere.h"
#include "drawcube.h"
#include "cylinderTexture.h"
struct Point {
    float x;
    float y;
    float z;
};
std::vector<float>cXALL;
std::vector<float>cZALL;
float cRALL = 6.5;


int flagDOLNA = 0;

bool keyFPressed = false;
bool keyGPressed = false;
bool key1Pressed = false;
bool key2Pressed = false;
bool key3Pressed = false;
bool key4Pressed = false;
bool key5Pressed = false;
bool key6Pressed = false;
bool keyF7Pressed = false;
bool keyF8Pressed = false;
bool keyF9Pressed = false;
bool keyF0Pressed = false;
std::vector<float>cX2;
std::vector<float>cZ2;
float cR2 = 6;
float dseatheta = 0.0;
int flagsd = -2;
int flagsd1 = -2;
float dseatheta100 = 0.0;


float rtheta = 0.0;
using namespace std;

const int MAX_DEPTH = 4; // Maximum recursion depth
const int CHILDREN_NUMBER = 7; // Number of branches per node

float cylinderVertices[] = {
    0.0f, 0.0f, 0.0f,  // Base center
    0.0f, 1.0f, 0.0f,  // Top center
    // Base circle
    0.5f, 0.0f, 0.0f,
    0.35f, 0.0f, 0.35f,
    -0.35f, 0.0f, 0.35f,
    -0.5f, 0.0f, 0.0f,
    -0.35f, 0.0f, -0.35f,
    0.35f, 0.0f, -0.35f,
    0.5f, 0.0f, 0.0f,
    // Top circle
    0.5f, 1.0f, 0.0f,
    0.35f, 1.0f, 0.35f,
    -0.35f, 1.0f, 0.35f,
    -0.5f, 1.0f, 0.0f,
    -0.35f, 1.0f, -0.35f,
    0.35f, 1.0f, -0.35f,
    0.5f, 1.0f, 0.0f
};

GLuint VAO, VBO;

void setupCylinder()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderVertices), cylinderVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
//void drawSwing(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float seatR = 0.8f, float seatG = 0.4f, float seatB = 0.2f, float ropeR = 0.5f, float ropeG = 0.5f, float ropeB = 0.5f);
void drawCylinder(const glm::mat4& model, GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection)
{
    glUseProgram(shaderProgram);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 8); // Base circle
    glDrawArrays(GL_TRIANGLE_FAN, 8, 8); // Top circle
    glBindVertexArray(0);
}

void drawFractalTree(const glm::mat4& m2w_noscale, int depth, Shader& shaderProgram, const glm::vec3& view, CylinderWithTexture* texturedCylinder ,CylinderWithTexture* texturedLeaf)
{
    glm::mat4 leaf = m2w_noscale * glm::scale(glm::mat4(1.0f), glm::vec3(0.6, 0.01f, 1.0) * glm::vec3(1.0f / depth));
    // Base case: stop recursion if maximum depth is reached
    if (depth >= MAX_DEPTH)
    {
        glm::mat4 leaf = m2w_noscale
            * glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 1.0f, 0.2f)) // Position leaf at the top
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.1f, 0.6f));    // Scale leaf appropriately
        texturedLeaf->drawCylinder(shaderProgram, leaf, view);
        leaf = m2w_noscale
            * glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 2.5f, 0.2f)) // Position leaf at the top
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.1f, 0.6f));    // Scale leaf appropriately
        texturedLeaf->drawCylinder(shaderProgram, leaf, view);
        leaf = m2w_noscale
            * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.2f)) // Position leaf at the top
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.1f, 0.6f));    // Scale leaf appropriately
        texturedLeaf->drawCylinder(shaderProgram, leaf, view);
        return;
    }
    
    // Scale the model matrix inversely proportional to the depth
    glm::mat4 m2w = m2w_noscale * glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 1.2f, 0.5)* glm::vec3(1.0f / depth));
    //texturedCylinder->drawCylinder(shaderProgram, m2w, view);
    if(depth!=1)m2w = glm::translate(m2w, glm::vec3(0.0, 4.0f, 0));
    // Draw the textured cylinder for the current branch
    texturedCylinder->drawCylinder(shaderProgram, m2w, view);
    //leaf = glm::translate(leaf, glm::vec3(0.0, 5.0f, 0));
    //texturedLeaf->drawCylinder(shaderProgram, leaf, view);


    for (int i = 0; i < CHILDREN_NUMBER; ++i)
    {
        // Calculate the rotation angle for each child branch
        float angle = (2.0f * glm::pi<float>() / CHILDREN_NUMBER) * i;

        // Create the translation and rotation transformations for the child branch
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 4.0f, 0) * glm::vec3(0, 1.0f / depth, 0));
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0))
            * glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.25f, glm::vec3(1, 0, 0));
        glm::mat4 rotation2 = glm::rotate(translation, angle, glm::vec3(0, 1, 0))
            * glm::rotate(glm::mat4(1.0f), glm::pi<float>() * 0.25f, glm::vec3(1, 0, 0));

        // Recursive call to draw the child branch
        drawFractalTree(m2w_noscale  * translation * rotation, depth + 1, shaderProgram, view, texturedCylinder, texturedLeaf);
    }
}



void tree(Shader& lightingShader, Sphere& sphere, float tx1, float ty1, float tz1, float sx1, float sy1, float sz1, CylinderWithTexture* texturedCylinder,  CylinderWithTexture* texturedLeaf, const glm::vec3 view);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void nagordola(unsigned int&bezierVAO,unsigned int &cubeVAO, Shader& lightingShader, Sphere &sphere, vector<float> &cx,vector<float>&cz,
    float tx1, float ty1,float tz1, float sx1, float sy1, float sz1
    );
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void processInput(GLFWwindow* window);
void dolna(unsigned int& bezierCylinder, unsigned int& bezierDolna, unsigned int& bezierDolna2, unsigned int& bezierMatha, unsigned int& cubeVAO, Shader& lightingShader,
    vector<float>& cx1, vector<float>& cz1,
    float cmnx, float cmny, float cmnz
);
void slipper(unsigned int& bezierVAO, unsigned int& cubeVAO, Shader& lightingShader, Sphere& sphere, vector<float>& cx, vector<float>& cz,
    float tx1, float ty1, float tz1, float sx1, float sy1, float sz1);
void singleDolna(unsigned int& cubeVAO, Cube& cube1, Shader& lightingShader, Shader& ligthingwithtexture, float tx, float ty, float tz);

void bera(unsigned int& cubeVAO, Cube& cube1, Shader& lightingShader, Shader& ligthingwithtexture, float tx, float ty, float tz,
    float rotateAngle, glm::vec3 rotationAxis);
long long nCr(int n, int r);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
void lamp(unsigned int& cubeVAO, Shader& lightingShader, float tx1, float ty1, float tz1, float sx1, float sy1, float sz1);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L);
void toyShop(unsigned int& cubeVAO, Cube& coffee, Cube& coffee1, Cube& coffee2, Shader& lightingShader, Shader& lightingShaderWithTexture, float txC, float tyC, float tzC);
void coffeeshop(unsigned int& cubeVAO, Cube& coffee, Cube& coffee1, Cube& coffee2, Shader& lightingShader, Shader& lightingShaderWithTexture, float txC, float tyC, float tzC);
void drawCubeWithTransformations(Shader& shader, Cube& cube, glm::vec3 translate, float rotation, glm::vec3 scale, glm::vec3 color);

void drawSwing(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f));
void drawSeat(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f));
void drawSphereMatrix(Shader& lightingShader, unsigned int sphereVAO, unsigned int& cubeVAO);


glm::mat4 transform(float tr_x, float tr_y, float tr_z, float rot_x, float rot_y, float rot_z, float scal_x, float scal_y, float scal_z) {
    // Modelling Transformation
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tr_x, tr_y, tr_z));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(scal_x, scal_y, scal_z));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    return model;
}


// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
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
float scale_Z = 1.0;
float dx = 0.000;
int  flag = 4;
int nightflag = 1;
int flagSD = 4;
float dy = 0.0000;
float dxx = 0.0000;
float dtheta = 0.0;
float dthetaY = 0.0;
int flagSeeSaw = 1;
float dtheta12 = 0.0;
int flagR = 0;
float dyL = 0.0;
int flagNagordola = 0;
int Ni = 0;
int tiTest = 0;
int flagTest = 1;
int flagRocket = 0;
float dthetaX = 0.0;
int rocketI = 0;


int r = 0;

int dolnaX[] = {599,1199,1799,2399,2999,3599};

//float txr = 0.0;
//float tyr = 0.0;
//float tzr = 0.0;

//control points
vector<float> cntrlPoints
{
    -0.1200, 1.9950, 5.1000,
    -0.3150, 1.8950, 5.1000,
    -0.4600, 1.7800, 5.1000,
    -0.6000, 1.6600, 5.1000,
    -0.7050, 1.5650, 5.1000,
    -0.8500, 1.4550, 5.1000,
    -0.9550, 1.3800, 5.1000,
    -1.0550, 1.2850, 5.1000,
};

std::vector<float> cntrlPointsSole = {
    -6.0f, 12.0f, 10.0f,   // Start (top left of slide)
    -4.5f, 8.5f, 10.0f,
    -4.5f, 8.5f, 10.0f,    // Mid curve (descending)
    -2.5f, 4.5f, 10.0f,    // Mid curve (descending)
     5.0f, -2.0f, 10.0f     // End (bottom right of slide)
};

// Control points for the slipper arch (Bezier curve/surface)
std::vector<float> cntrlPointsArch = {
    -2.5f, 0.0f, 0.5f,   // Left base of the arch
    -1.5f, 1.5f, 0.8f,   // Left peak
     0.0f, 2.0f, 1.0f,   // Topmost peak of the arch
     1.5f, 1.5f, 0.8f,   // Right peak
     2.5f, 0.0f, 0.5f    // Right base of the arch
};

vector<float> cntrlPointsSlipper
{
    -0.1200, 1.9950, 5.1000,
    -0.3150, 1.8950, 5.1000,
    -0.4600, 1.7800, 5.1000,
    -0.6000, 1.6600, 5.1000,
    -0.7050, 1.5650, 5.1000,
    -0.8500, 1.4550, 5.1000,
    -0.9550, 1.3800, 5.1000,
    -1.0550, 1.2850, 5.1000,
};

vector<float> cntrlForDolna
{
    -0.6700, 1.7400, 5.1000,
    -0.6650, 1.5000, 5.1000,
    -0.5750, 1.2950, 5.1000,
    -0.4150, 1.1700, 5.1000,
    -0.2350, 1.0550, 5.1000,
};
vector<float> cntrlPointsCylinder
{
-0.4500, 1.7950, 5.1000,
-0.4550, 1.4950, 5.1000,
-0.4550, 1.0000, 5.1000,
};
vector<float> cntrlPointsForNagordola2
{
    -0.800, 0.700, 5.1000,
    -0.7500, 0.10, 5.1000,
    -0.70, -0.25, 5.1000,
};

std::vector<float> cntrlPointsForNagordola = {
    // Base of the cart
    -0.8f, -0.25f, 0.0f,   // Bottom-left
    0.8f, -0.25f, 0.0f,    // Bottom-right

    // Sides of the cart
    -0.8f, 1.2f, 0.0f,   // Top-left corner of the base
    0.8f, 1.2f, 0.0f,    // Top-right corner of the base

    // Canopy arc (approximated with control points)
    -0.8f, 1.2f, 0.0f,   // Left edge of the arc
    -0.4f, 1.6f, 0.0f,   // Mid-left arc
    0.4f, 1.6f, 0.0f,    // Mid-right arc
    0.8f, 1.2f, 0.0f     // Right edge of the arc
};

vector<float> cntrlPointstree
{
    
    
-0.2500, 1.8350, 5.1000,
-0.4700, 1.6950, 5.1000,
-0.7050, 1.4400, 5.1000,
-0.6600, 1.2550, 5.1000,
-0.4150, 1.2000, 5.1000,

    
};
vector<float> cntrlCyCurve
{
 
};

vector<float> cntrlmatha
{

    -0.0200, 1.9100, 5.1000,
    -0.1850, 1.7550, 5.1000,
    -0.3300, 1.5600, 5.1000,

};



vector<int> nagordolaSeatIndex = { 719, 1439, 2159, 2879, 3599 };
float irt = 0.0;
float irtz = 0.0;

vector <float> coordinates;
vector <float> normals;
vector <int> indices;
vector <float> vertices;
class point
{
public:
    point()
    {
        x = 0;
        y = 0;
    }
    int x;
    int y;
} clkpt[2];
int mouseButtonFlag = 0;
FILE* fp;
const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 20;
bool showControlPoints = true;
bool loadBezierCurvePoints = false;
bool showHollowBezier = false;
bool lineMode = false;
unsigned int bezierVAO;
unsigned int bezierSlipper;
unsigned int bezierSole;
unsigned int bezierArch;
unsigned int bezierCylinderVAO;
unsigned int bezierForNagordola;
unsigned int bezierForNagordola2;
unsigned int bezierTree;
unsigned int bezierCyCurve;
unsigned int bezierMatha;
unsigned int bezierDolna;

// camera
Camera camera(glm::vec3(25, 5.0f, -6.6 + 0.25f + 60.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);


// positions of the point lights

glm::vec3 pointLightPositions[] = {
    glm::vec3(100.0f,  25.0f,   25.0f),
    glm::vec3(-10.0f,  25.0f,   -32.0f),
    glm::vec3(80.0f, 25.0f, -32.0f),
    glm::vec3(-20.0f,  25.0f,   25.0f),
    
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.014f,  //k_l
    0.0007, //k_q
    1       // light number
);

PointLight pointlight3(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.014f,  //k_l
    0.0007, //k_q
    2       // light number
);



PointLight pointlight4(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.014f,  //k_l
    0.0007, //k_q
    3       // light number
);
PointLight pointlight5(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.014f,  //k_l
    0.0007, //k_q
    4       // light number
);



DirLight dirLight(
        -0.2f, -1.0f, -0.3f,
        0.05f, 0.05f, 0.05f,
        .5f, 0.5f, 0.5f,     // diffuse
        0.5f, 0.5f, 0.5f
);


SpotLight spotLight(
    -3.50f, 34.0f, -2.0f,
    0.6f, -1.0f, 0.5f,
    0.5f, 0.0f, 0.5f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,
    0.09f,
    0.032f,
    12.5f,
    15.0f
);



// light settings
bool PointToggle1 = true;
bool PointToggle3 = true;
bool PointToggle4 = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 1.5f;    // time between current frame and last frame
float lastFrame = 0.0f;


int main()
{

    // glfw: initialize and configure
    // ------------------------------
    int ti = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    //Shader lightingShader("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, //back side of cube, surface normal on -z 
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
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


    unsigned int cubeVAO, cubeVBO, cubeEBO; 
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO); 

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //cylinder sd points
    std::vector<Point> cylinder;
    //cylinder.push_back({ 0, 0, -8 });
    float radius1 = 2.0;
    float radius = 1.0;
    for (float theta = 0; theta <= 360; theta += 1) {
        float x = radius * glm::cos(glm::radians(theta));
        float y = radius * glm::sin(glm::radians(theta));

        float x1 = radius1 * glm::cos(glm::radians(theta));
        float y1 = radius1 * glm::sin(glm::radians(theta));

        cylinder.push_back({ x, y, 0.0f });
        cylinder.push_back({ x1, y1, -2.0f }); // normal cylinder
        //cylinder.push_back({ x * .5f, y * .5f, -8.0f }); // cylinder with different circle radius
    }

    unsigned int CVBO, CVAO;
    glGenVertexArrays(1, &CVAO);
    glGenBuffers(1, &CVBO);

    glBindVertexArray(CVAO);

    glBindBuffer(GL_ARRAY_BUFFER, CVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct Point) * cylinder.size(), cylinder.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Point), (void*)0);
    glEnableVertexAttribArray(0);
    //end of sd cylinder point

    //CYLINDER CIRCLE POINTS START
    std::vector<Point> cylindercircle;
    cylindercircle.push_back({ 0, 0, -8 });
    float radiusC = 6.0;
    //float radius = 1.0;
    for (float theta = 0; theta <= 360; theta += 1) {
        float x = radiusC * glm::cos(glm::radians(theta));
        float y = radiusC * glm::sin(glm::radians(theta));

        //float x1 = radius1 * glm::cos(glm::radians(theta));
        //float y1 = radius1 * glm::sin(glm::radians(theta));

        cylindercircle.push_back({ x, y, -8.0f });
        //cylinder.push_back({ x1, y1, -2.0f }); // normal cylinder
        //cylinder.push_back({ x * .5f, y * .5f, -8.0f }); // cylinder with different circle radius
    }

    unsigned int CcVBO, CcVAO;
    glGenVertexArrays(1, &CcVAO);
    glGenBuffers(1, &CcVBO);

    glBindVertexArray(CcVAO);

    glBindBuffer(GL_ARRAY_BUFFER, CcVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct Point) * cylindercircle.size(), cylindercircle.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Point), (void*)0);
    glEnableVertexAttribArray(0);
    //end of sd cylinderCIRCLE point




    //cylinder for rider shade
    std::vector<Point> cylinderShade;
    //cylinder.push_back({ 0, 0, -8 });
    float radiusSh = 3.0;
    for (float theta = 0; theta <= 360; theta += 1) {
        float x = radiusSh * glm::cos(glm::radians(theta));
        float y = radiusSh * glm::sin(glm::radians(theta));

        cylinderShade.push_back({ x, y, 0.0f });
        cylinderShade.push_back({ x, y, -2.0f }); // normal cylinder
        //cylinder.push_back({ x * .5f, y * .5f, -8.0f }); // cylinder with different circle radius
    }

    unsigned int CSVBO, CSVAO;
    glGenVertexArrays(1, &CSVAO);
    glGenBuffers(1, &CSVBO);

    glBindVertexArray(CSVAO);

    glBindBuffer(GL_ARRAY_BUFFER, CSVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct Point) * cylinderShade.size(), cylinderShade.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Point), (void*)0);
    glEnableVertexAttribArray(0);


    bezierVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();


    bezierSlipper = hollowBezier(cntrlPointsSlipper.data(), ((unsigned int)cntrlPointsSlipper.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierCylinderVAO = hollowBezier(cntrlPointsCylinder.data(), ((unsigned int)cntrlPointsCylinder.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierArch = hollowBezier(cntrlPointsArch.data(), ((unsigned int)cntrlPointsArch.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();


    bezierSole = hollowBezier(cntrlPointsSole.data(), ((unsigned int)cntrlPointsSole.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierForNagordola = hollowBezier(cntrlPointsForNagordola.data(), ((unsigned int)cntrlPointsForNagordola.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierForNagordola2 = hollowBezier(cntrlPointsForNagordola2.data(), ((unsigned int)cntrlPointsForNagordola2.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierTree = hollowBezier(cntrlPointstree.data(), ((unsigned int)cntrlPointstree.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierCyCurve = hollowBezier(cntrlCyCurve.data(), ((unsigned int)cntrlCyCurve.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();

    bezierMatha = hollowBezier(cntrlmatha.data(), ((unsigned int)cntrlmatha.size() / 3) - 1);
    coordinates.clear();
    normals.clear();
    indices.clear();
    vertices.clear();
    
    bezierDolna = hollowBezier(cntrlForDolna.data(), ((unsigned int)cntrlForDolna.size() / 3) - 1);



    //points for move an object in circle
    std::vector<float>cX;
    std::vector<float>cZ;
    float cR = 6;
    for (float itheta = 0;itheta <= 360;itheta = itheta + 0.1)
    {
        float x = cR * glm::cos(glm::radians(itheta));
        float z = cR * glm::sin(glm::radians(itheta));
        cX.push_back(x);
        cZ.push_back(z);
    }
    for (float itheta = 0;itheta <= 360;itheta = itheta + 0.1)
    {
        float x = cRALL * glm::cos(glm::radians(itheta));
        float z = cRALL * glm::sin(glm::radians(itheta));
        cXALL.push_back(x);
        cZALL.push_back(z);
    }
   /* std::cout << cX.size() << endl;
    std::cout << cZ.size() << endl;*/
    std::vector<float>cXt;
    std::vector<float>cZt;
    float cR1 = 6;
    for (float itheta = 1;itheta <= 360;itheta = itheta + 1.0)
    {
        float x = cR1 * glm::cos(glm::radians(itheta));
        float z = cR1 * glm::sin(glm::radians(itheta));
        cXt.push_back(x);
        cZt.push_back(z);
    }
    cout << "cXt = " << cXt.size() << endl;
    

    // render loop
    // -----------


    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    string diffuseMapPath = "./container2.jpg";
    string specularMapPath = "./container2.jpg";
    string diffuseMapRoad = "./road.jpg";
    string specularMapRoad = "./road.jpg";
    string diffuseMapSky = "./sky.jpg";
    string specularMapSky = "./sky.jpg";
    string diffuseMapBrick = "./colr.jpg";
    string specularMapBrick = "./colr.jpg";
    string diffuseMapColor = "./color.jpg";
    string specularMapColor = "./color.jpg";
    string diffuseMapSofa = "./k1.jpg";
    string specularMapSofa = "./k1.jpg";
    string diffuseMapWater =  "./block.jpg";
    string specularMapWater = "./block.jpg";
    string diffuseMapCoffee = "./carnival2.jpg";
    string specularMapCoffee = "./carnival2.jpg";
    string woodenWalls = "./newWall.jpg";

    string diffuseMapCoffeeNes = "./carnival2.jpg";
    string specularMapCoffeeNes = "./carnival2.jpg";

    string diffuseMapWood = "./wood.jpg";
    string specularMapWood = "./wood.jpg";

    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap1 = loadTexture(diffuseMapRoad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap1 = loadTexture(specularMapRoad.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap2 = loadTexture(diffuseMapSky.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap2 = loadTexture(specularMapSky.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap3 = loadTexture(diffuseMapBrick.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap3 = loadTexture(specularMapBrick.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap4 = loadTexture(diffuseMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapSofa.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap5 = loadTexture(diffuseMapColor.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapColor.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap6 = loadTexture(diffuseMapWater.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap6 = loadTexture(specularMapWater.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap7 = loadTexture(diffuseMapCoffee.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap7 = loadTexture(specularMapCoffee.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap8 = loadTexture(diffuseMapCoffee.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap8 = loadTexture(specularMapCoffee.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int diffMap9 = loadTexture(diffuseMapCoffeeNes.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap9 = loadTexture(specularMapCoffeeNes.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
 

    unsigned int diffMap11 = loadTexture(diffuseMapWood.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap11 = loadTexture(specularMapWood.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    unsigned int woodenWall = loadTexture(woodenWalls.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
 

    Cube road = Cube(diffMap1, specMap1, 32.0f, 0.0f, 0.0f, 2.0f, 2.0f);
    Cube water = Cube(diffMap6, specMap6, 32.0f, 0.0f, 0.0f, 100.0f, 100.0f);
    Cube brick = Cube(diffMap3, specMap3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube sky = Cube(diffMap2, specMap2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube sofa = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube color = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube coffee = Cube(diffMap7, specMap7, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube coffeeT = Cube(diffMap8, specMap8, 32.0f, 0.0f, 0.0f, 3.0f, 1.0f);
    Cube coffeT2 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    Cube wood = Cube(diffMap11, specMap11, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube wooden = Cube(woodenWall, woodenWall, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
 
    //std::cout << diffMap << ' ' << specMap << std::endl;
    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 20.0f, 20.0f);
    Cube cube1 = Cube();
    Sphere sphere = Sphere(0.8);
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
        glClearColor(0.53f, 0.80f, 0.98f, 0.05f);
        if (nightflag == 2)
        {
            glClearColor(0.0f,0.0f,0.0f, 0.08f);
            

        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        //pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);
        // point light 4
        pointlight4.setUpPointLight(lightingShader);
        pointlight5.setUpPointLight(lightingShader);
        dirLight.setUpDirLight(lightingShader);
        spotLight.setUpSpotLight(lightingShader);
        lightingShader.use();


        //light right
        glm::mat4 modelight = glm::mat4(1.0f);
        modelight = glm::translate(modelight, glm::vec3(100.0f, 25.0f, 25.0f));
        modelight = glm::scale(modelight, glm::vec3(0.50f, -40.0f, 0.5f)); // Make it a smaller cube
        lightingShader.setMat4("model", modelight);
        lightingShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        drawCube(cubeVAO, lightingShader, modelight, 0.5f, 0.3f, 0.0f);

        modelight = glm::mat4(1.0f);
        modelight = glm::translate(modelight, glm::vec3(-20.0f, 25.0f, 25.0));
        modelight = glm::scale(modelight, glm::vec3(0.50f, -40.0f, 0.5f)); // Make it a smaller cube
        lightingShader.setMat4("model", modelight);
        lightingShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        drawCube(cubeVAO, lightingShader, modelight, 0.0f, 0.0f, 0.0f);



        //light right
        modelight = glm::mat4(1.0f);
        modelight = glm::translate(modelight, glm::vec3(80, 25.0f, -32.0));
        modelight = glm::scale(modelight, glm::vec3(0.6f, -40.0f, 0.6f)); // Make it a smaller cube
        lightingShader.setMat4("model", modelight);
        lightingShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        drawCube(cubeVAO, lightingShader, modelight, 0.5f, 0.2f, 0.0f);


        //light left
        modelight = glm::mat4(1.0f);
        modelight = glm::translate(modelight, glm::vec3(-10, 25.0f, -32.0f));
        modelight = glm::scale(modelight, glm::vec3(0.50f, -40.0f, 0.5f)); // Make it a smaller cube
        lightingShader.setMat4("model", modelight);
        lightingShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        drawCube(cubeVAO, lightingShader, modelight, 0.0f, 0.0f, 0.0f);
        




        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 250.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);


        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);




        nagordola(bezierForNagordola,cubeVAO, lightingShader,sphere, cX, cZ, 15+5,-1,35,0-1.5,0-1.5,0-1.5 );
        slipper(bezierForNagordola, cubeVAO, lightingShader, sphere, cX, cZ, 20 + 5, -3, 20, 0 - 1, 0 - 1, 0 - 1);
        drawSphereMatrix(lightingShader, bezierForNagordola, cubeVAO);


        // camera/view transformation
        float degree = 0;
        glm::mat4 view = camera.GetViewMatrix();
        float r = glm::length(camera.Position - glm::vec3(view[3]));
        ourShader.setMat4("view", view);
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        



        //.............. we now draw as many light bulbs as we have point lights.................//
        glBindVertexArray(lightCubeVAO);
        ourShader.use();
        for (unsigned int i = 0; i < 4; i++)
        {
            // Reset the model matrix
            glm::mat4 model = glm::mat4(1.0f);

            // Position the sphere at the light position
            model = glm::translate(model, pointLightPositions[i]);

            // Scale the sphere to match the desired size
            model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Adjust scaling as needed

            // Set the color (if needed, depending on the shader implementation)
            lightingShader.use();
            lightingShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // White color for the sphere

            // Draw the sphere using the provided drawSphere function

            sphere.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
            sphere.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
            sphere.specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
            sphere.shininess = 32.0f;
            sphere.drawSphere(lightingShader, model);
        }
   

        //coffeeshop(cubeVAO, coffeeT,coffee,coffeT2, lightingShader, lightingShaderWithTexture, 0.0, 0.0, 0.0);
        toyShop(cubeVAO, coffeeT, coffee, coffeT2, lightingShader, lightingShaderWithTexture, 0.0, 0.0, 0.0);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);
        //2nd part of cube draw
        lightingShaderWithTexture.use();
        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        // point light 4
        pointlight4.setUpPointLight(lightingShaderWithTexture);

        // ghash surface
        glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.5, -2.5, -48.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(90, .5, 70));
        modelMatrixForContainer = translateMatrix * scaleMatrix;
        cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        
 
        // mati surface
        modelMatrixForContainer = glm::mat4(1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-400.5, -2.6, -47.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1100, .5, 700));
        modelMatrixForContainer = translateMatrix * scaleMatrix;
        //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
        water.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

     

        int beraI = 0;
        int beraII=0;
        bera( cubeVAO, wooden, lightingShader, lightingShaderWithTexture, -45, 0, 32 + 3, 0.0f, glm::vec3(0, 1, 0));
        for (int i = 0; i < 5; i++)
        {
            bera(cubeVAO, wooden, lightingShader, lightingShaderWithTexture, -45 + beraI, 0, 32 + 3, 0.0f, glm::vec3(0, 1, 0));
            beraI = beraI + 7;

        }
        beraI = beraI + 1 * 7;
        for (int i = 5; i < 12; i++)
        {
            bera(cubeVAO, wooden, lightingShader, lightingShaderWithTexture, -40 + beraI, 0, 32 + 3, 0.0f, glm::vec3(0, 1, 0));
            beraI = beraI + 7;

        }
        bera( cubeVAO, wooden, lightingShader, lightingShaderWithTexture, -50, 0, 30 - beraII, -45.0f, glm::vec3(0, 1, 0));
        for (int i = 0; i < 10; i++)
        {
            bera(cubeVAO, wooden, lightingShader, lightingShaderWithTexture, -50, 0, 30 - beraII, 90.0f, glm::vec3(0, 1, 0));
            beraII += 7; // Adjust position for the next bera
        }
        beraII = 0;
        bera(cubeVAO, wooden, lightingShader, lightingShaderWithTexture, 56, 0, 30 - beraII, -135.0f, glm::vec3(0, 1, 0));
        for (int i = 0; i < 10; i++)
        {
            bera( cubeVAO, wooden, lightingShader, lightingShaderWithTexture, 56, 0, 30 - beraII, 90.0f, glm::vec3(0, 1, 0));
            beraII += 7; // Adjust position for the next bera
        }

        singleDolna(cubeVAO, cube1, lightingShader, lightingShaderWithTexture, 0.0, -4.0, -35.0);
        singleDolna(cubeVAO, cube1, lightingShader, lightingShaderWithTexture, 8.0, -4.0, -35.0);


        model = glm::mat4(1.0f);
        dolna(bezierCylinderVAO, bezierForNagordola, bezierForNagordola2, bezierMatha, cubeVAO, lightingShader, cX, cZ, -40.0, 0.0f, -5.0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(25, 0, 23)); // Position the cylinder
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the cylinder

        CylinderWithTexture* texturedCylinder = new CylinderWithTexture(
            1.5f, 1.5f, 20.0f,  // Base radius, top radius, height
            36, 1,             // Sector count, stack count
            glm::vec3(0.2f, 0.2f, 0.2f),  // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),  // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f),  // Specular
            32.0f,                        // Shininess
            woodenWall, woodenWall      // Textures
        );
        texturedCylinder->drawCylinder(lightingShaderWithTexture, model, camera.Position);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35, 0, 23)); // Position the cylinder
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the cylinder
        texturedCylinder->drawCylinder(lightingShaderWithTexture, model, camera.Position);

        CylinderWithTexture* texturedCone = new CylinderWithTexture(
            1.5f, 0.1f, 3.0f,  // Base radius, top radius, height
            36, 1,             // Sector count, stack count
            glm::vec3(0.2f, 0.2f, 0.2f),  // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),  // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f),  // Specular
            32.0f,                        // Shininess
            woodenWall, woodenWall      // Textures
        );

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(25, 11, 23)); // Position the cylinder
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the cylinder
        texturedCone->drawCylinder(lightingShaderWithTexture, model, camera.Position);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35, 11, 23)); // Position the cylinder
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the cylinder
        texturedCone->drawCylinder(lightingShaderWithTexture, model, camera.Position);

        CylinderWithTexture* texturedTree = new CylinderWithTexture(
            0.8f, 0.3f, 10.0f,  // Base radius, top radius, height
            36, 1,             // Sector count, stack count
            glm::vec3(0.2f, 0.2f, 0.2f),  // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),  // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f),  // Specular
            32.0f,                        // Shininess
            woodenWall, woodenWall      // Textures
        );
        //texturedTree->drawCylinder(lightingShaderWithTexture, model, camera.Position);

        CylinderWithTexture* texturedLeaf = new CylinderWithTexture(
            0.8f, 0.8f, 0.8f,  // Base radius, top radius, height
            36, 1,             // Sector count, stack count
            glm::vec3(0.2f, 0.2f, 0.2f),  // Ambient
            glm::vec3(0.8f, 0.8f, 0.8f),  // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f),  // Specular
            32.0f,                        // Shininess
            diffMap, specMap      // Textures
        );
        //texturedLeaf->drawCylinder(lightingShaderWithTexture, model, camera.Position);


        tree(lightingShaderWithTexture, sphere, -25.0f, 00.0f, -10.0f, 1.5f, 1.5f, 1.5f, texturedTree, texturedLeaf, camera.Position);
        tree(lightingShaderWithTexture, sphere, 70.0f, 00.0f, -30.0f, 2.0f, 2.0f, 2.0f, texturedTree, texturedLeaf, camera.Position);

        tree(lightingShaderWithTexture, sphere, 38.0f, -2.0f, 28.0f, .5f, .5f, .5f, texturedTree, texturedLeaf, camera.Position);
        tree(lightingShaderWithTexture, sphere, 23.0f, -2.0f, 28.0f, .5f, 0.5f, 0.5f, texturedTree, texturedLeaf, camera.Position);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!keyFPressed) // Only toggle if the key wasn't already pressed
        {
            if (flagNagordola)flagNagordola = 0; // Toggle the flag
            else flagNagordola = 1;
            keyFPressed = true; // Mark the key as pressed
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        keyFPressed = false; // Reset the key state when released
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (!keyGPressed) // Only toggle if the key wasn't already pressed
        {
            flagDOLNA = !flagDOLNA; // Toggle the flag
            keyGPressed = true; // Mark the key as pressed
        }

    }
    else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
    {
        keyGPressed = false; // Reset the key state when released
    }
    

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (!key1Pressed) // Only toggle if the key wasn't already pressed
        {
            if(dirLight.on)
            {
                dirLight.on = false;
                dirLight.turnOff();
            }
            else {
                dirLight.on = true;
                dirLight.turnOn();
            }
            key1Pressed = true; // Mark the key as pressed
        }

    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
    {
        key1Pressed = false; // Reset the key state when released
    }

    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
    {
        pointlight1.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
    {
        pointlight3.turnOn();
    }



    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (!key4Pressed) // Only toggle if the key wasn't already pressed
        {
            if (spotLight.on)
            {
                spotLight.on = false;
                spotLight.turnOff();
            }
            else {
                spotLight.on = true;
                spotLight.turnOn();
            }
            key4Pressed = true; // Mark the key as pressed
        }

    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
    {
        key4Pressed = false; // Reset the key state when released
    }
    

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
       // nightflag = 1;
        pointlight1.turnOn();
        pointlight3.turnOn();
        pointlight4.turnOn();
        pointlight5.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS )
    {
       // nightflag = 2;
        pointlight1.turnOff();
        pointlight3.turnOff();
        pointlight4.turnOff();
        pointlight5.turnOff();

    }

  

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }
  
    
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        pointlight1.turnDiffuseOn();
        pointlight3.turnDiffuseOn();
        spotLight.turnDiffuseOn();
        dirLight.turnDiffuseOn();

    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        
        /*pointlight1.turnOff();
        pointlight3.turnOff();
        dirLight.turnOff();
        spotLight.turnOff();*/

        pointlight1.turnSpecularOn();
        pointlight3.turnSpecularOn();
        spotLight.turnSpecularOn();
        dirLight.turnSpecularOn();

    }


  

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        spotLight.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        spotLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        pointlight1.turnAmbientOn();
        pointlight3.turnAmbientOn();
        spotLight.turnAmbientOn();
        dirLight.turnAmbientOn();
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        pointlight1.turnSpecularOn();
        pointlight3.turnSpecularOn();
        spotLight.turnSpecularOn();
        dirLight.turnSpecularOn();
    }



    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
    {
        pointlight1.turnOn();

    }

    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
    {
        pointlight1.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
    {
        pointlight3.turnOn();
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
    {
        pointlight3.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
    {
        pointlight4.turnOn();
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
    {
        pointlight4.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
    {
        pointlight5.turnOn();
    }

    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
    {
        pointlight5.turnOff();

    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        nightflag = 1;
        pointlight1.turnOn();
        pointlight3.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        nightflag = 2;
        pointlight1.turnOff();
        pointlight3.turnOff();

    }
    

    


    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {

        pointlight4.turnOn();
        cout << "on" << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        pointlight1.turnDiffuseOn();
        pointlight3.turnDiffuseOn();
        spotLight.turnDiffuseOn();
        dirLight.turnDiffuseOn();

    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {

        pointlight1.turnOff();
        pointlight3.turnOff();
        dirLight.turnOff();
        spotLight.turnOff();

    }


    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        dirLight.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        dirLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        spotLight.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        spotLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        pointlight1.turnAmbientOn();
        pointlight3.turnAmbientOn();
        spotLight.turnAmbientOn();
        dirLight.turnAmbientOn();
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        pointlight1.turnSpecularOn();
        pointlight3.turnSpecularOn();
        spotLight.turnSpecularOn();
        dirLight.turnSpecularOn();
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
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


long long nCr(int n, int r)
{
    if (r > n / 2)
        r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
{
    double y = 0;
    double x = 0;
    t = t > 1.0 ? 1.0 : t;
    for (int i = 0; i < L + 1; i++)
    {
        long long ncr = nCr(L, i);
        double oneMinusTpow = pow(1 - t, double(L - i));
        double tPow = pow(t, double(i));
        double coef = oneMinusTpow * tPow * ncr;
        x += coef * ctrlpoints[i * 3];
        y += coef * ctrlpoints[(i * 3) + 1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

unsigned int hollowBezier(GLfloat ctrlpoints[], int L)
{
    int i, j;
    float x, y, z, r;                //current coordinates
    float theta;
    float nx, ny, nz, lengthInv;    // vertex normal


    const float dtheta = 2 * pi / ntheta;        //angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];

    for (i = 0; i <= nt; ++i)              //step through y
    {
        BezierCurve(t, xy, ctrlpoints, L);
        r = xy[0];
        y = xy[1];
        theta = 0;
        t += dt;
        lengthInv = 1.0 / r;

        for (j = 0; j <= ntheta; ++j)
        {
            double cosa = cos(theta);
            double sina = sin(theta);
            z = r * cosa;
            x = r * sina;

            coordinates.push_back(x);
            coordinates.push_back(y);
            coordinates.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            // center point of the circle (0,y,0)
            nx = (x - 0) * lengthInv;
            ny = (y - y) * lengthInv;
            nz = (z - 0) * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            theta += dtheta;
        }
    }

    // generate index list of triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1

    int k1, k2;
    for (int i = 0; i < nt; ++i)
    {
        k1 = i * (ntheta + 1);     // beginning of current stack
        k2 = k1 + ntheta + 1;      // beginning of next stack

        for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
        {
            // k1 => k2 => k1+1
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // k1+1 => k2 => k2+1
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    size_t count = coordinates.size();
    for (int i = 0; i < count; i += 3)
    {
        vertices.push_back(coordinates[i]);
        vertices.push_back(coordinates[i + 1]);
        vertices.push_back(coordinates[i + 2]);

        vertices.push_back(normals[i]);
        vertices.push_back(normals[i + 1]);
        vertices.push_back(normals[i + 2]);
    }

    unsigned int bezierVAO;
    glGenVertexArrays(1, &bezierVAO);
    glBindVertexArray(bezierVAO);

    // create VBO to copy vertex data to VBO
    unsigned int bezierVBO;
    glGenBuffers(1, &bezierVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
    glBufferData(GL_ARRAY_BUFFER,                   // target
        (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
        vertices.data(),   // ptr to vertex data
        GL_STATIC_DRAW);                   // usage

    // create EBO to copy index data
    unsigned int bezierEBO;
    glGenBuffers(1, &bezierEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
        (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
        indices.data(),               // ptr to index data
        GL_STATIC_DRAW);                   // usage

    // activate attrib arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // set attrib arrays with stride and offset
    int stride = 24;     // should be 24 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

    // unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return bezierVAO;
}


void railstation(unsigned int& bezierVAO, unsigned int& bezierCylinderVAO, unsigned int& cubeVAO, Shader& lightingShader, float x, float y, float z, float r, float g, float bl)
{
    glm::mat4 identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrixCurve, rotateXMatrixCurve, rotateYMatrixCurve, rotateZMatrixCurve, scaleMatrixCurve, modelforCurve;
    translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(x-10,y-2,z+20));
    rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(3, 2.5, 3));
    modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
    lightingShader.setMat4("model", modelforCurve);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLE_FAN,(unsigned int)indices.size(),GL_UNSIGNED_INT,(void*)0);                       
    glBindVertexArray(0);

    glm::mat4 identityMatrixCurve1 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrixCurve1, rotateXMatrixCurve1, rotateYMatrixCurve1, rotateZMatrixCurve1, scaleMatrixCurve1, modelforCurve1;
    translateMatrixCurve1 = glm::translate(identityMatrixCurve1, glm::vec3(58.0f, -1.9f, -20.5f));
    rotateXMatrixCurve1 = glm::rotate(identityMatrixCurve1, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrixCurve1 = glm::rotate(identityMatrixCurve1, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrixCurve1 = glm::rotate(identityMatrixCurve1, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrixCurve1 = glm::scale(identityMatrixCurve1, glm::vec3(3,2.5,3));
    modelforCurve1 = translateMatrixCurve1 * rotateXMatrixCurve1 * rotateYMatrixCurve1 * rotateZMatrixCurve1 * scaleMatrixCurve1;
    lightingShader.setMat4("model", modelforCurve1);
    glBindVertexArray(bezierCylinderVAO);
    glDrawElements(GL_TRIANGLE_FAN,(unsigned int)indices.size(),GL_UNSIGNED_INT,(void*)0);                       
    glBindVertexArray(0);

    glm::mat4 identityMatrixCube = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(61, y+3, -8.0f));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrixCube = glm::rotate(identityMatrixCube, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(7, 0.5, 10));
    modelforCube = translateMatrixCube * rotateXMatrixCube * rotateYMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    drawCube(cubeVAO, lightingShader, modelforCube, 1.0, 1.0, 1.0);


}

void drawCubeWithTransformations(Shader& shader, Cube& cube, glm::vec3 translate, float rotation, glm::vec3 scale, glm::vec3 color) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::translate(identityMatrix, translate);
    glm::mat4 rotateMatrix = glm::rotate(identityMatrix, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 scaleMatrix = glm::scale(identityMatrix, scale);
    glm::mat4 modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;

    //cube.drawCube(shader, modelMatrix, color.r, color.g, color.b);
    cube.drawCubeWithMaterialisticProperty(shader, modelMatrix);
    //cube.drawCube(shader, modelMatrix);
}

void nagordola(unsigned int& bezierVAO, unsigned int& cubeVAO, Shader& lightingShader, Sphere &sphere, vector<float>& cx, vector<float>& cz,
    float tx1, float ty1, float tz1, float sx1, float sy1, float sz1){


    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);


    Cube cube = Cube();
    glm::mat4 identityMatrixSphere = glm::mat4(1.0f);
    glm::mat4 translateMatrixSphere, rotateXMatrixSphere, rotateYMatrixSphere, rotateZMatrixSphere, scaleMatrixSphere, modelforSphere;
    translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(38.0f+tx1+cx[0], 7.8f +0.8, tz1 - 22.5f));
    scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(0.5,0.5,0.5));
    modelforSphere = translateMatrixSphere * scaleMatrixSphere;
    lightingShader.setMat4("model", modelforSphere);
    
    //sphere.drawSphere(lightingShader, modelforSphere);
    cube.setMaterialisticProperty(glm::vec3(0.902f, 0.447f, 0.145f), glm::vec3(0.902f, 0.447f, 0.145f), glm::vec3(0.902f, 0.447f, 0.145f), 32.0f);
    identityMatrixSphere = glm::mat4(1.0f);
    translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(38.0f + tx1 + cx[0], 7.8f +0.8, tz1 - 19.5f));
    scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(0.5, 0.5, 0.5));
    modelforSphere = translateMatrixSphere * scaleMatrixSphere;
    lightingShader.setMat4("model", modelforSphere);
    /*lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
    //sphere.drawSphere(lightingShader, modelforSphere);


    glm::mat4 identityMatrixCube = glm::mat4(1.0f);
    glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], 7.6f + 0.8, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.5, 0.5, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    /*lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
    //cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], 7.6f + 0.8, tz1 - 19.5f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube,glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    /*lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
    //cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], 7.6f + 0.8, tz1 - 22.8f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    /*lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
    //cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], 7.6f + 0.8, tz1 - 19.5f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-330.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    /*lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
   // cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], 7.6f + 0.8, tz1 - 22.8f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-330.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    /*lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(0.902f, 0.447f, 0.145f));
    lightingShader.setFloat("material.shininess", 32.0f);*/
    //cube.setMaterialisticProperty(glm::vec3(0.902f, 0.447f, 0.145f), glm::vec3(0.902f, 0.447f, 0.145f), glm::vec3(0.902f, 0.447f, 0.145f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    
    //final
    glm::vec3 translate = glm::vec3(38.0f + tx1 + cx[0], 7.6f + 1.0f, tz1 - 20.7f);
    glm::vec3 scale = glm::vec3(6.0f, 0.25f, 0.15f);
    glm::vec3 color = glm::vec3(0.902f, 0.447f, 0.145f); // Red color

    // Draw cubes with different rotations
    std::vector<float> rotations = { 0.0f, 72.0f, 144.0f, 216.0f, 288.0f, 360.0f };
    for (float rotation : rotations) {
        drawCubeWithTransformations(lightingShader, cube, translate, rotation + rtheta, scale, color);
    }

    

    // Common transformation parameters
    glm::mat4 identityMatrixCurve = glm::mat4(1.0f);
    glm::mat4 rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(3 + sx1, 2.5 + sy1, 3 + sz1));

    // Shader setup (material properties)
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(0.902f, 0.447f, 0.145f)); 
    lightingShader.setVec3("material.diffuse", glm::vec3(0.902f, 0.447f, 0.145f));
    lightingShader.setVec3("material.specular", glm::vec3(0.902f, 0.447f, 0.145f));
    lightingShader.setFloat("material.shininess", 32.0f);

    // Iterate through indices and apply transformations
    for (int index : nagordolaSeatIndex) {
        glm::vec3 translation = glm::vec3(tx1 + 38.0f + cx[index] + cx[0],
            ty1*sy1 + 7.0f + cz[index] + cz[0],
            tz1 - 21.5f);

        glm::mat4 translateMatrixCurve = glm::translate(identityMatrixCurve, translation);
        glm::mat4 scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(3, 1.5, 2.5));
        glm::mat4 modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;


        drawSeat(cubeVAO, lightingShader, modelforCurve);
    
    }

    if (flagNagordola)
    {
        //nagordolaSeatIndex[0]++;
        for (int idx = 0; idx < nagordolaSeatIndex.size(); idx++) {
            nagordolaSeatIndex[idx]+=10;
            nagordolaSeatIndex[idx] %= 3600;
        }
        rtheta = 1 + rtheta;
    }

}
    
    


void singleDolna(unsigned int& cubeVAO, Cube& cube1, Shader& lightingShader, Shader& ligthingwithtexture, float tx, float ty, float tz)
{
    Cube cube = Cube();
    glm::mat4 identityMatrixCube = glm::mat4(1.0f);
    glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx, ty+10.5, tz+10.0 ));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-15.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.20, -9.5, .20));
    modelforCube = translateMatrixCube * rotateXMatrixCube *scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(15.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.20, -9.5, .20));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f+7 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-15.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.20, -9.5, .20));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);


    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f+7 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(15.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.20, -9.5, .20));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    //uprer handle
    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.9f + 7 + tx, ty+10.5, tz + 10.0));
    //rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(15.0f), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(-7, 0.20, 0.20));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    //dolna seat handle
    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 1 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-15.0f+dseatheta), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.15, -6.5, .15));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,0.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);


    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 1 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(15.0f+dseatheta), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.15, -6.5, .15));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 6 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-15.0f+dseatheta), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.15, -6.5, .15));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);


    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 6 + tx, ty+10.5, tz + 10.0));
    rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(15.0f+dseatheta), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(.15, -6.5, .15));
    modelforCube = translateMatrixCube * rotateXMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    //dolna seat
    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 0.5 + tx , ty+4.0, tz + 8.0 ));
    //rotateXMatrixCube = glm::rotate(identityMatrixCube, glm::radians(dseatheta), glm::vec3(1.0, 0.0, 0.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(6.0, 0.20, 4.00));
    modelforCube = translateMatrixCube  * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    //if (flagsd == 2)
    //{
    //    dseatheta = dseatheta - 0.07;
    //    if (dseatheta <= -6.0)
    //    {
    //        flagsd = -2;
    //    }
    //}
    //if (flagsd == -2)
    //{
    //    dseatheta = dseatheta + 0.07;
    //    //cout << dseatheta << endl;
    //    if (dseatheta >= 6.0)
    //    {
    //        flagsd = 2;
    //    }

    //}

    //if (flagsd1 == 2)
    //{
    //    dseatheta100 = dseatheta100 - (0.008);
    //    cout << "hello" << endl;
    //    if (dseatheta100 <= -0.30)
    //    {
    //        flagsd1 = -2;
    //    }
    //}
    //if (flagsd1 == -2)
    //{
    //    dseatheta100 = dseatheta100 + 1.00;
    //    
    //    /*cout << dseatheta100 << endl;
    //    if (dseatheta100 >= 0.3)
    //    {
    //        flagsd1 = 2;
    //    }*/

    //}
    


}



void toyShop(unsigned int& cubeVAO, Cube& coffee, Cube& coffee1, Cube& coffee2, Shader& lightingShader, Shader& lightingShaderWithTexture, float txC, float tyC, float tzC)
{
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    model = glm::mat4(1.0f);
    model = transform(2.0, -1.9, -25.0, 0.0, 0.0, 0.0,  14.0f, 12.00f, 1.0f);
    coffee1.drawCubeWithTexture(lightingShaderWithTexture, model);

    model = glm::mat4(1.0f);
    model = transform(2.0, -1.9, -18.0, 0.0, 0.0, 0.0, 14.00f, 4.00f, 3.0f);
    lightingShader.setMat4("model", model);
    drawCube(cubeVAO, lightingShader, model, 1.0f, 0.001f, 0.001f);


    model = glm::mat4(1.0f);
    model = transform(2.0, -1.9 + 12 - 0.5, -25.00, 0.0, 0.0, 0.0, 14.0f, 0.50f, 12.0f);
    //lightingShader.setMat4("model", model);
    //drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
    coffee1.drawCubeWithTexture(lightingShaderWithTexture, model);

    model = glm::mat4(1.0f);
    model = transform(2.0, -1.9, -25.10, 0.0, -90.0, 0.0, 12.0f, 12.00f, 0.50f);
    coffee1.drawCubeWithTexture(lightingShaderWithTexture, model);

    model = glm::mat4(1.0f);
    model = transform(16.5, -1.9, -25.0, 0.0, -90.0, 0.0, 12.0f, 12.00f, 0.50f);
    coffee1.drawCubeWithTexture(lightingShaderWithTexture, model);

}


void slipper(unsigned int& bezierVAO, unsigned int& cubeVAO, Shader& lightingShader, Sphere& sphere, vector<float>& cx, vector<float>& cz,
    float tx1, float ty1, float tz1, float sx1, float sy1, float sz1) {

    Cube cube = Cube();
    glm::mat4 identityMatrixSphere = glm::mat4(1.0f);
    glm::mat4 translateMatrixSphere, rotateXMatrixSphere, rotateYMatrixSphere, rotateZMatrixSphere, scaleMatrixSphere, modelforSphere;
    translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(38.0f + tx1 + cx[0], ty1 + 7.8f + 0.8, tz1 - 22.5f));
    scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(0.5, 0.5, 0.5));
    modelforSphere = translateMatrixSphere * scaleMatrixSphere;
    lightingShader.setMat4("model", modelforSphere);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    sphere.drawSphere(lightingShader, modelforSphere);

    identityMatrixSphere = glm::mat4(1.0f);
    translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(38.0f + tx1 + cx[0], ty1 + 7.8f + 0.8, tz1 - 19.5f));
    scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(0.5, 0.5, 0.5));
    modelforSphere = translateMatrixSphere * scaleMatrixSphere;
    lightingShader.setMat4("model", modelforSphere);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    sphere.drawSphere(lightingShader, modelforSphere);


    glm::mat4 identityMatrixCube = glm::mat4(1.0f);
    glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.5, 0.5, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f+0.5 + tx1 + cx[0], ty1 + 7.0f + 0.8, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.15, 0.15, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 1.8 + tx1 + cx[0], ty1 + 7.0f - 1.6, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.15, 0.15, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 2.9 + tx1 + cx[0], ty1 + 7.0f - 3.2, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.15, 0.15, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);


    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + 3.9 + tx1 + cx[0], ty1 + 7.0f - 4.8, tz1 - 19.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.15, 0.15, -3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);



    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 19.5f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-48.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(-0.40, -10.5, 0.15));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);


    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 19.5f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-48.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.050, -10.5, -3.2));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 22.8f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-48.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(-0.40, -10.5, 0.15));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), glm::vec3(0.588f, 0.141f, 0.345f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 19.5f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-330.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    //glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(37.8f + tx1 + cx[0], ty1 + 7.6f + 0.8, tz1 - 22.8f));
    rotateZMatrixCube = glm::rotate(identityMatrixCube, glm::radians(-330.0f), glm::vec3(0.0, 0.0, 1.0));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.30, -10.5, 0.30));
    modelforCube = translateMatrixCube * rotateZMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

}

void lamp(unsigned int& cubeVAO, Shader& lightingShader, float tx1, float ty1, float tz1, float sx1, float sy1, float sz1)
{
    Sphere sphere = Sphere(1.0f,  36,  18,  glm::vec3(1.0f, 1.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f),  32.0f);
    Cube cube = Cube();
    glm::mat4 identityMatrixSphere = glm::mat4(1.0f);
    glm::mat4 translateMatrixSphere, rotateXMatrixSphere, rotateYMatrixSphere, rotateZMatrixSphere, scaleMatrixSphere, modelforSphere;
    translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(-11.0f+0.1+tx1, 8.5f+ty1, 23.5f+tz1));
    scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(1.0, 1.0, 1.0));
    modelforSphere = translateMatrixSphere * scaleMatrixSphere;
    lightingShader.setMat4("model", modelforSphere);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    sphere.drawSphere(lightingShader, modelforSphere);

    glm::mat4 identityMatrixCube = glm::mat4(1.0f);
    glm::mat4 translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(-11.250f+0.15+tx1, 8.5f+ty1, 23.5f+tz1));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(0.5, -7.5, 0.5));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(-11.250f -1.20+tx1, 7.70f+ty1, 21.5f+tz1));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(3.0, -0.25, 3.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    /*identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(-11.250f - 1.60, 7.70f-5.25+1-5, 21.5f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(4.0, -0.25, 4.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);

    identityMatrixCube = glm::mat4(1.0f);
    translateMatrixCube, rotateXMatrixCube, rotateYMatrixCube, rotateZMatrixCube, scaleMatrixCube, modelforCube;
    translateMatrixCube = glm::translate(identityMatrixCube, glm::vec3(-11.250f - 2.20, 7.70f - 4.60-5.0, 20.50f));
    scaleMatrixCube = glm::scale(identityMatrixCube, glm::vec3(5.0, -0.25, 5.0));
    modelforCube = translateMatrixCube * scaleMatrixCube;
    lightingShader.setMat4("model", modelforCube);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);
    cube.setMaterialisticProperty(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 32.0f);
    cube.drawCubeWithMaterialisticProperty(lightingShader, modelforCube);*/
}

void drawCurvySlipper(unsigned int& bezierVAO, unsigned int& cubeVAO, Shader& lightingShader, Sphere& sphere, vector<float>& cx, vector<float>& cz,
    float tx1, float ty1, float tz1, float sx1, float sy1, float sz1) {

    Cube cube = Cube();
    
    // Common transformation parameters
    glm::mat4 identityMatrixCurve = glm::mat4(1.0f);
    glm::mat4 rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(3 + sx1, 2.5 + sy1, 3 + sz1));

    // Shader setup (material properties)
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);

    // Iterate through indices and apply transformations
    
        glm::vec3 translation = glm::vec3(tx1 + 38.0f + cx[0],
            ty1 + 5.0f + cz[0],
            tz1 - 20.5f);

        glm::mat4 translateMatrixCurve = glm::translate(identityMatrixCurve, translation);
        glm::mat4 modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;

        // Render nagordola (red)
        //lightingShader.setMat4("model", modelforCurve);
        //glBindVertexArray(bezierArch);
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
        //glBindVertexArray(0);



        // Render nagordola2 (blue)
        lightingShader.setMat4("model", modelforCurve);
        glBindVertexArray(bezierSole);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);


    

    //if (flagNagordola)
    //{
    //    //nagordolaSeatIndex[0]++;
    //    for (int idx = 0; idx < nagordolaSeatIndex.size(); idx++) {
    //        nagordolaSeatIndex[idx]++;
    //        nagordolaSeatIndex[idx] %= 3600;
    //    }
    //    rtheta = 0.1 + rtheta;
    //}

}


unsigned int generateSlideVAO(std::vector<GLfloat>& controlPoints, int resolution)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate points along the Bezier curve
    float xy[2];
    for (int i = 0; i <= resolution; i++)
    {
        float t = float(i) / resolution;
        BezierCurve(t, xy, controlPoints.data(), controlPoints.size() / 3 - 1);
        vertices.push_back(xy[0]);
        vertices.push_back(xy[1]);
        vertices.push_back(0.0f); // z-coordinate
    }

    // Generate indices for the curve (line strip)
    for (int i = 0; i < resolution; i++)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Create VAO and VBO
    unsigned int slideVAO, slideVBO, slideEBO;
    glGenVertexArrays(1, &slideVAO);
    glBindVertexArray(slideVAO);

    glGenBuffers(1, &slideVBO);
    glBindBuffer(GL_ARRAY_BUFFER, slideVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &slideEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slideEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return slideVAO;
}

void renderSlide(unsigned int slideVAO, Shader& shader, int resolution)
{
    shader.use();
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix
    shader.setMat4("model", model);

    glBindVertexArray(slideVAO);
    glDrawElements(GL_LINES, resolution * 2, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}



void tree(Shader& lightingShader, Sphere& sphere, float tx1, float ty1, float tz1, float sx1, float sy1, float sz1, CylinderWithTexture* texturedCylinder, CylinderWithTexture* texturedLeaf, const glm::vec3 view)
{
    // Initialize the base model-to-world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(tx1, ty1, tz1));
    model = glm::scale(model, glm::vec3(sx1, sy1, sz1));

    // Pass shader properties
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);

    // Draw the fractal tree recursively using the textured cylinder
    drawFractalTree(model, 1, lightingShader, view,  texturedCylinder, texturedLeaf);
}

void bera(unsigned int& cubeVAO, Cube& cube1, Shader& lightingShader, Shader& ligthingwithtexture, float tx, float ty, float tz,
    float rotateAngle, glm::vec3 rotationAxis)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, modelLathi;
    modelLathi = glm::mat4(1.0f);


    identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    modelLathi = glm::mat4(1.0f);
    identityMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(modelLathi, glm::vec3(tx + 34.0f, ty + 4.0f, tz - 11.5f));
    rotateXMatrix = glm::rotate(modelLathi, glm::radians(rotateAngle), rotationAxis);
    scaleMatrix = glm::scale(modelLathi, glm::vec3(7.0f, 1.0f, 0.1f));
    modelLathi = translateMatrix * rotateXMatrix * scaleMatrix;
    cube1.drawCubeWithTexture(ligthingwithtexture, modelLathi);


    identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    //glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, modelLathi;
    modelLathi = glm::mat4(1.0f);
    identityMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(modelLathi, glm::vec3(tx + 34.0f, ty - 0.5f, tz - 11.5f));
    rotateXMatrix = glm::rotate(modelLathi, glm::radians(rotateAngle), rotationAxis);
    scaleMatrix = glm::scale(modelLathi, glm::vec3(7.0f, 1.0f, 0.1f));
    modelLathi = translateMatrix * rotateXMatrix * scaleMatrix;
    cube1.drawCubeWithTexture(ligthingwithtexture, modelLathi);


    modelLathi = glm::mat4(1.0f);
    identityMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(modelLathi, glm::vec3(tx + 34.5f, ty - 2.0f, tz - 11.5f));
    rotateXMatrix = glm::rotate(modelLathi, glm::radians(rotateAngle), rotationAxis);
    scaleMatrix = glm::scale(modelLathi, glm::vec3(6.0f, 8.0f, 0.3f));
    modelLathi = translateMatrix * rotateXMatrix * scaleMatrix;
    cube1.drawCubeWithTexture(ligthingwithtexture, modelLathi);
}




void dolna(unsigned int& bezierCylinder, unsigned int& bezierDolna,  unsigned int& bezierDolna2, unsigned int& bezierMatha, unsigned int& cubeVAO, Shader& lightingShader,
    vector<float>& cx1, vector<float>& cz1,
    float cmnx, float cmny, float cmnz
)
{
    glm::mat4 identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrixCurve, rotateXMatrixCurve, rotateYMatrixCurve, rotateZMatrixCurve, scaleMatrixCurve, modelforCurve;
    identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(37.5f + cmnx, 8.0f + cmny, 8.5f + cmnz));
    rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(1.0f, 1.0f, 1.0f));
    scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(20, 0.8, 20));
    modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
    lightingShader.setMat4("model", modelforCurve);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(1, 0.459, 0));
    lightingShader.setVec3("material.diffuse", glm::vec3(1, 0.459, 0));
    lightingShader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
    lightingShader.setFloat("material.shininess", 25.0f);
    glBindVertexArray(bezierCylinderVAO);
    glDrawElements(GL_TRIANGLE_FAN, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    //glm::mat4 translateMatrixCurve, rotateXMatrixCurve, rotateYMatrixCurve, rotateZMatrixCurve, scaleMatrixCurve, modelforCurve;
    identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(37.5f + cmnx, -6.5 + cmny, 8.5f + cmnz));
    rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(1.0f, 1.0f, 1.0f));
    scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(10, 10.0, 10));
    modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
    lightingShader.setMat4("model", modelforCurve);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(0.839, 0.827, 0.961));
    lightingShader.setVec3("material.diffuse", glm::vec3(0.839, 0.827, 0.961));
    lightingShader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
    lightingShader.setFloat("material.shininess", 25.0f);
    glBindVertexArray(bezierMatha);
    glDrawElements(GL_TRIANGLE_FAN, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);


    identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    //glm::mat4 translateMatrixCurve, rotateXMatrixCurve, rotateYMatrixCurve, rotateZMatrixCurve, scaleMatrixCurve, modelforCurve;
    identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(37.5f + cmnx, -16.5 + cmny, 8.5f + cmnz));
    rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(1.0f, 1.0f, 1.0f));
    rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(1.0f, 1.0f, 1.0f));
    scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(1, 15.0, 1));
    modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
    lightingShader.setMat4("model", modelforCurve);
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(0.522f, 0.804f, 0.969f));
    lightingShader.setVec3("material.diffuse", glm::vec3(0.522f, 0.804f, 0.969f));
    lightingShader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
    lightingShader.setFloat("material.shininess", 25.0f);
    glBindVertexArray(bezierCylinder);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    glm::mat4 cylinderPosition = translateMatrixCurve;


    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, modelLathi;
    modelLathi = glm::mat4(1.0f);
    identityMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(modelLathi, glm::vec3(28.2f + cmnx, -2.5f + cmny, 2.5f + cmnz));
    //rotateXMatrix = glm::rotate(identityMatrix, glm::radians(50.0f + dthetaX), glm::vec3(1.0f, 0.0f, 0.0f));
    //rotateYMatrix = glm::rotate(identityMatrix, glm::radians(00.0f + dthetaY), glm::vec3(0.0f, 1.0f, 0.0f));
    //rotateZMatrix = glm::rotate(identityMatrix, glm::radians(00.0f + dthetaY), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(modelLathi, glm::vec3(19, 1.0, 14));
    modelLathi = translateMatrix * scaleMatrix;
    lightingShader.setMat4("model", modelLathi);
    drawCube(cubeVAO, lightingShader, modelLathi, 0.969, 0.792, 0.671);



   
    const int numDolnas = 6; // Number of dolnas
    
    for (int i = 0; i < numDolnas; ++i) {
        identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(34.0f - 2.5 + cmnx + cx1[0] + cx1[dolnaX[i]], 1.0f + cmny, 9.5f + cmnz + cz1[0] + cz1[dolnaX[i]]));
        rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(2, 2, 2));
        modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
        lightingShader.setMat4("model", modelforCurve);
        lightingShader.use();
        lightingShader.setVec3("material.ambient", glm::vec3(0.5f, 0.0f, 0.5f));
        lightingShader.setVec3("material.diffuse", glm::vec3(0.5f, 0.0f, 0.5f));
        lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("material.shininess", 25.0f);
        glBindVertexArray(bezierDolna);
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);


        identityMatrixCurve = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        translateMatrixCurve = glm::translate(identityMatrixCurve, glm::vec3(34.0f - 2.5 + cmnx + cx1[0] + cx1[dolnaX[i]], 0.5f + cmny, 9.5f + cmnz + cz1[0] + cz1[dolnaX[i]]));
        rotateXMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrixCurve = glm::rotate(identityMatrixCurve, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrixCurve = glm::scale(identityMatrixCurve, glm::vec3(3, 2, 3));
        modelforCurve = translateMatrixCurve * rotateXMatrixCurve * rotateYMatrixCurve * rotateZMatrixCurve * scaleMatrixCurve;
        lightingShader.setMat4("model", modelforCurve);
        lightingShader.use();
        lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 0.0f));
        lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 0.0f));
        lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("material.shininess", 25.0f);
        glBindVertexArray(bezierDolna2);
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);

        identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        modelLathi = glm::mat4(1.0f);
        identityMatrix = glm::mat4(1.0f);
        translateMatrix = glm::translate(modelLathi, glm::vec3(33.8f + cmnx + cx1[0] + cx1[dolnaX[i]] - 2.5, 3.5f + cmny, 9.5f + cmnz + cz1[0] + cz1[dolnaX[i]]));
        //rotateXMatrix = glm::rotate(identityMatrix, glm::radians(50.0f + dthetaX), glm::vec3(1.0f, 0.0f, 0.0f));
        //rotateYMatrix = glm::rotate(identityMatrix, glm::radians(00.0f + dthetaY), glm::vec3(0.0f, 1.0f, 0.0f));
        //rotateZMatrix = glm::rotate(identityMatrix, glm::radians(00.0f + dthetaY), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(modelLathi, glm::vec3(0.15f, 5.5f, 0.15f));
        modelLathi = translateMatrix * scaleMatrix;
        lightingShader.setMat4("model", modelLathi);
        drawCube(cubeVAO, lightingShader, modelLathi, 0.3f, 0.0f, 0.3f);
    }

   
    if (flagDOLNA == 1)
    {
 
        for (int i = 0; i < 6; i++) {
            dolnaX[i]+=5;
            dolnaX[i] %= 3600;
        }
    }


}


void drawSwing(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model)
{
    lightingShader.use(); // Activate the shader

    // Seat of the swing
    glm::mat4 seatModel = model;
    seatModel = glm::translate(seatModel, glm::vec3(0.0f, -0.5f, 0.0f)); // Position the seat
    seatModel = glm::scale(seatModel, glm::vec3(1.5f, 0.1f, 1.0f));      // Scale to make it rectangular
    lightingShader.setVec3("material.ambient", glm::vec3(0.8f, 0.0f, 0.2f)); // Seat color
    drawCube(cubeVAO, lightingShader, seatModel);

    // Left rope
    glm::mat4 leftRopeModel = model;
    leftRopeModel = glm::translate(leftRopeModel, glm::vec3(0.0f, 0.0f, 0.5f)); // Position the left rope
    leftRopeModel = glm::scale(leftRopeModel, glm::vec3(0.05f, 4.0f, 0.05f));    // Thin and tall for the rope
    lightingShader.setVec3("material.ambient", glm::vec3(0.5f, 0.5f, 0.5f)); // Rope color
    drawCube(cubeVAO, lightingShader, leftRopeModel);

    // Right rope
    glm::mat4 rightRopeModel = model;
    rightRopeModel = glm::translate(rightRopeModel, glm::vec3(1.5f, 0.0f, 0.5f)); // Position the right rope
    rightRopeModel = glm::scale(rightRopeModel, glm::vec3(0.05f, 4.0f, 0.05f));   // Thin and tall for the rope
    drawCube(cubeVAO, lightingShader, rightRopeModel);

    // Left armrest
    glm::mat4 leftArmRestModel = model;
    leftArmRestModel = glm::translate(leftArmRestModel, glm::vec3(0.0f, -0.4f, 0.0f)); // Position left armrest
    leftArmRestModel = glm::scale(leftArmRestModel, glm::vec3(0.1f, 0.5f, 1.0f));       // Scale to make it thin and long
    lightingShader.setVec3("material.ambient", glm::vec3(0.6f, 0.3f, 0.2f)); // Armrest color
    drawCube(cubeVAO, lightingShader, leftArmRestModel);

    // Right armrest
    glm::mat4 rightArmRestModel = model;
    rightArmRestModel = glm::translate(rightArmRestModel, glm::vec3(1.4f, -0.4f, 0.0f)); // Position right armrest
    rightArmRestModel = glm::scale(rightArmRestModel, glm::vec3(0.1f, 0.5f, 1.0f));      // Scale to make it thin and long
    drawCube(cubeVAO, lightingShader, rightArmRestModel);

    // Backrest
    glm::mat4 backRestModel = model;
    backRestModel = glm::translate(backRestModel, glm::vec3(0.0f, -0.5f, 0.0f)); // Position backrest behind the seat
    backRestModel = glm::scale(backRestModel, glm::vec3(1.5f, 1.0f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, backRestModel);
}



void drawSeat(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model)
{
    lightingShader.use(); // Activate the shader

    float r = 0.910f, g = 0.871f, b = 0.125f;

    // Seat of the swing
    glm::mat4 seatModel = model;
    seatModel = glm::translate(seatModel, glm::vec3(-0.5f, -0.5f, 0.0f)); // Position the seat
    seatModel = glm::scale(seatModel, glm::vec3(1.0f, 0.1f, 1.0f));      // Scale to make it rectangular
    lightingShader.setVec3("material.ambient", glm::vec3(0.902f, 0.447f, 0.145f)); // Seat color
    lightingShader.setVec3("material.diffuse", glm::vec3(0.902f, 0.447f, 0.145f)); // Seat color
    lightingShader.setVec3("material.specular", glm::vec3(0.902f, 0.447f, 0.145f)); // Seat color
    drawCube(cubeVAO, lightingShader, seatModel, r, g, b);


    // Left armrest
    glm::mat4 leftArmRestModel = model;
    leftArmRestModel = glm::translate(leftArmRestModel, glm::vec3(-0.5f, -0.4f, 0.0f)); // Position left armrest
    leftArmRestModel = glm::scale(leftArmRestModel, glm::vec3(0.1f, 0.5f, 1.0f));       // Scale to make it thin and long
    lightingShader.setVec3("material.ambient", glm::vec3(0.6f, 0.3f, 0.2f)); // Armrest color
    drawCube(cubeVAO, lightingShader, leftArmRestModel, r, g, b);

    // Right armrest
    glm::mat4 rightArmRestModel = model;
    rightArmRestModel = glm::translate(rightArmRestModel, glm::vec3(0.4f, -0.4f, 0.0f)); // Position right armrest
    rightArmRestModel = glm::scale(rightArmRestModel, glm::vec3(0.1f, 0.5f, 1.0f));      // Scale to make it thin and long
    drawCube(cubeVAO, lightingShader, rightArmRestModel , r, g, b);

    // Backrest
    glm::mat4 backRestModel = model;
    backRestModel = glm::translate(backRestModel, glm::vec3(-0.5f, -0.5f, 0.0f)); // Position backrest behind the seat
    backRestModel = glm::scale(backRestModel, glm::vec3(1.0f, 0.2f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, backRestModel, r, g, b);
    backRestModel = glm::translate(backRestModel, glm::vec3(-0.0f, 1.5f, 0.0f)); // Position backrest behind the seat
   // backRestModel = glm::scale(backRestModel, glm::vec3(1.0f, 0.2f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, backRestModel, r, g, b);
    backRestModel = glm::translate(backRestModel, glm::vec3(-0.0f, 1.5f, 0.0f)); // Position backrest behind the seat
    // backRestModel = glm::scale(backRestModel, glm::vec3(1.0f, 0.2f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, backRestModel, r, g, b);
    backRestModel = glm::translate(backRestModel, glm::vec3(-0.0f, 1.5f, 0.0f)); // Position backrest behind the seat
    // backRestModel = glm::scale(backRestModel, glm::vec3(1.0f, 0.2f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, backRestModel, r, g, b);
    

    // Frontrest
    glm::mat4 FrontRestModel = model;
    FrontRestModel = glm::translate(FrontRestModel, glm::vec3(-0.5f, -0.5f, 0.9f)); // Position backrest behind the seat
    FrontRestModel = glm::scale(FrontRestModel, glm::vec3(1.0f, 0.2f, 0.1f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, FrontRestModel, r, g, b);
    FrontRestModel = glm::translate(FrontRestModel, glm::vec3(-0.0f, 1.5f, 0.0f));;       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, FrontRestModel, r, g, b);
    FrontRestModel = glm::translate(FrontRestModel, glm::vec3(-0.0f, 1.5f, 0.0f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, FrontRestModel, r, g, b);
    FrontRestModel = glm::translate(FrontRestModel, glm::vec3(-0.0f, 1.5f, 0.0f));       // Scale to make it a flat vertical panel
    drawCube(cubeVAO, lightingShader, FrontRestModel, r, g, b);
}

void drawSphereMatrix(Shader& lightingShader, unsigned int sphereVAO, unsigned int& cubeVAO) {
    Sphere sphere = Sphere();
    glm::mat4 identityMatrixSphere = glm::mat4(1.0f);
    float sphereSpacingX = 0.5f; // Distance between spheres in the x-direction
    float sphereSpacingY = 0.5f; // Distance between spheres in the y-direction
    float startX = 25.0f;       // Starting x-position of the matrix (adjust for 20 columns)
    float startY = 0.0f;         // Starting y-position of the matrix
    float fixedZ = -18.0f;        // Fixed z-position of the spheres
    float sphereScale = 0.25f;    // Scale for each sphere
    

    glm::mat4 backModel = glm::mat4(1.0f);
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::translate(identityMatrix, glm::vec3(startX-0.75, startY -2.0, fixedZ-0.5f));
    glm::mat4 scaleMatrix = glm::scale(identityMatrix, glm::vec3(6.0f, 13.0f, 0.01f));
    backModel = translateMatrix * scaleMatrix;
    lightingShader.setMat4("model", backModel);
    drawCube(cubeVAO, lightingShader, backModel, 0.0f, 0.0f, 0.0f);

    // Define colors for each row
    std::vector<glm::vec3> rowColors = { 
      glm::vec3(0.5f, 0.0f, 0.5f), // Purple
      glm::vec3(0.5f, 0.0f, 0.5f), // Purple
      glm::vec3(1.0f, 0.4f, 0.7f), // Pink
      glm::vec3(1.0f, 0.4f, 0.7f), // Pink
      glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
      glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
      glm::vec3(0.0f, 0.5f, 0.0f), // Dark Green
      glm::vec3(0.0f, 0.5f, 0.0f),  // Dark Green
      glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
      glm::vec3(1.0f, 1.0f, 0.0f), // Yellow

    };

    lightingShader.use();

    for (int row = 0; row < 20; ++row) {
        glm::vec3 rowColor = rowColors[row % rowColors.size()]; // Cycle through colors if rows exceed predefined colors
        lightingShader.setVec3("material.ambient", rowColor);
        lightingShader.setVec3("material.diffuse", rowColor);
        lightingShader.setVec3("material.specular", rowColor); // Specular color remains white
        lightingShader.setFloat("material.shininess", 12.0f);
        sphere.ambient = rowColor;
        sphere.diffuse = rowColor;
        sphere.specular = rowColor;


        for (int col = 0; col < 10; ++col) {
            glm::mat4 translateMatrixSphere = glm::translate(identityMatrixSphere, glm::vec3(
                startX + col * sphereSpacingX, // Align horizontally along x-axis
                startY + row * sphereSpacingY, // Align vertically along y-axis
                fixedZ                        // Keep z-axis fixed
            ));
            glm::mat4 scaleMatrixSphere = glm::scale(identityMatrixSphere, glm::vec3(sphereScale));
            glm::mat4 modelSphere = translateMatrixSphere * scaleMatrixSphere;

            lightingShader.setMat4("model", modelSphere);
            sphere.drawSphere(lightingShader, modelSphere); // Draw the sphere
        }
    }
}


