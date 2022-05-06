#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f,3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float move_x = 0.0f;
float move_z = 0.0f;
float moveAdjustment = 0.1;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(10.0f, 10.0f, 10.0f);
glm::vec3 lightColour(1.0f, 1.0f, 1.0f);

// grid things
int grid_dim = 16;

glm::vec3 blob_scale = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 grid_scale = 0.5f * blob_scale;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("../Resources/shader.vert", "../Resources/shader.frag");
    Shader terrainShader("../Resources/terrain_shader.vert", "../Resources/terrain_shader.frag");

    // load models
    // -----------
    Model ourModel1("../Resources/blob/blob.obj");

    float terrainVertices[] = {
            -1.0f, 0.0f, -1.0f,
            1.0f, 0.0f, -1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, -1.0f
    };

    // VAO and VBO of the ground
    unsigned int VAO_terrain, VBO_terrain;
    glGenVertexArrays(1, &VAO_terrain);
    glGenBuffers(1, &VBO_terrain);

    glBindVertexArray(VAO_terrain);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("lightColor", lightColour);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(move_x, 0.0f, move_z)); // translate it down so it's at the center of the scene
//        model = glm::scale(model,  blob_scale);	// it's a bit too big for our scene, so scale it down
//        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        ourShader.setMat4("model", model);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        //ourModel1.Draw(ourShader);

        // transformations for the grid
        glm::mat4 terrain_model = glm::mat4(1.0f);
        terrain_model = glm::scale(terrain_model, grid_scale);
        terrain_model = glm::translate(terrain_model, glm::vec3(-(float)grid_dim, 0.0f, -(float)grid_dim));

        // activate the terrain shader
        terrainShader.use();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);

        glBindVertexArray(VAO_terrain);

        // drawing the grid
        float val1, val2;
        for (int i = 0; i < grid_dim; i++) {
            terrain_model = glm::translate(terrain_model, glm::vec3(0.0f, 0.0f, 2.0f));
            // colour things
            if (i % 2 == 0) {
                val1 = 1.0f;
                val2 = 0.0f;
            } else {
                val1 = 0.0f;
                val2 = 1.0f;
            }
            for (int j = 0; j < grid_dim; j++) {
                // colour things cont
                if (j % 2 == 0) {
                    terrainShader.setVec4("aColour", glm::vec4(val1, 0.0f, 0.0f, 1.0f));
                } else {
                    terrainShader.setVec4("aColour", glm::vec4(val2, 0.0f, 0.0f, 1.0f));
                }
                terrain_model = glm::translate(terrain_model, glm::vec3(2.0f,  0.0f, 0.0f));
                terrainShader.setMat4("model", terrain_model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            terrain_model = glm::translate(terrain_model, glm::vec3(-(float)grid_dim * 2.0f, 0.0f, 0.0f));
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        move_x -= moveAdjustment;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        move_x += moveAdjustment;
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        move_z -= moveAdjustment;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        move_z += moveAdjustment;
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

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}