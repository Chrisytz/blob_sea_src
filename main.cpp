#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <stb_image.h>
#include <camera.h>
#include <iostream>
#include <mesh.h>
#include <model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera things
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// movement things
float move_x = 0.0f;
float move_z = 0.0f;
float moveAdjustment = 0.25f;

// cube transformation things
glm::vec3 blob_scale = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 grid_scale = 0.5f * blob_scale;

int grid_dim = 16;

void draw_grid(glm::mat4 terrain_model, Shader TerrainShader);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    // build and compile our shader program
    // ------------------------------------
    Shader BlobShader("../Resources/shader_blob.vert", "../Resources/shader_blob.frag");
    Shader TerrainShader("../Resources/shader_terrain.vert", "../Resources/shader_terrain.frag");

    // doing texture things
    // --------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../Resources/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float terrainVertices[] = {
            -1.0f, 0.0f, -1.0f,
            1.0f, 0.0f, -1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, -1.0f
    };

    float blobVertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    unsigned int VBO_blob, VAO_blob, VBO_terrain, VAO_terrain;

    // VAO and VBO of the cube
    glGenVertexArrays(1, &VAO_blob);
    glGenBuffers(1, &VBO_blob);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_blob);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_blob);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blobVertices), blobVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAO and VBO of the ground
    glGenVertexArrays(1, &VAO_terrain);
    glGenBuffers(1, &VBO_terrain);

    glBindVertexArray(VAO_terrain);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures
        glBindTexture(GL_TEXTURE_2D, texture);

        // transformation things
        // ---------------------

        // think of this as zooming things? i think?
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        // this is like the camera
        glm::mat4 view;
        view = camera.GetViewMatrix();

        // movement of the box itself
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, blob_scale);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
        model = glm::translate(model, glm::vec3(move_x, 0.0f, move_z));

        // transformations for the grid
        glm::mat4 terrain_model = glm::mat4(1.0f);
        terrain_model = glm::scale(terrain_model, grid_scale);
        terrain_model = glm::translate(terrain_model, glm::vec3(-(float)grid_dim, 0.0f, -(float)grid_dim));

        // activating shaders
        // ------------------

        // activate the blob shader
        BlobShader.use();
        BlobShader.setMat4("projection", projection);
        BlobShader.setMat4("view", view);
        BlobShader.setMat4("model", model);

        // render the blob triangles
        glBindVertexArray(VAO_blob);
        glDrawArrays(GL_TRIANGLES,0, 36);

        // activate the terrain shader
        TerrainShader.use();
        TerrainShader.setMat4("projection", projection);
        TerrainShader.setMat4("view", view);

        // render the blob triangles
        glBindVertexArray(VAO_terrain);

        // drawing the grid
        draw_grid(terrain_model, TerrainShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_blob);
    glDeleteVertexArrays(1, &VAO_terrain);
    glDeleteBuffers(1, &VBO_blob);
    glDeleteBuffers(1, &VBO_terrain);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void draw_grid(glm::mat4 terrain_model, Shader TerrainShader) {
    float val1, val2;
    // rows
    for (int i = 0; i < grid_dim; i++) {
        // setting val1 and val2 to get the checkered pattern
        if (i % 2 == 0) {
            val1 = 1.0f;
            val2 = 0.0f;
        } else {
            val1 = 0.0f;
            val2 = 1.0f;
        }
        // columns
        for (int j = 0; j < grid_dim; j++) {
            // setting the uniform to its colour
            if (j % 2 == 0) {
                TerrainShader.setVec4("aColour", val1, 0.0f, 0.0f, 1.0f);
            } else {
                TerrainShader.setVec4("aColour", val2, 0.0f, 0.0f, 1.0f);
            }
            // moving each square
            terrain_model = glm::translate(terrain_model, glm::vec3(2.0f,  0.0f, 0.0f));
            TerrainShader.setMat4("model", terrain_model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // moving to the next row
        terrain_model = glm::translate(terrain_model, glm::vec3(-(float)grid_dim * 2.0f, 0.0f, 2.0f));
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    // for the first time the function is called, we set lastX and lastY to the mouse position
    // there is no offset since this is the initial position
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // once we have the initial mouse position we calculate the x and y offsets
    // and then set the last x and y position to the current mouse position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    const float cameraSpeed = 0.005f;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}