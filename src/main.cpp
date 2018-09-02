#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <vector>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920 * 2;
const unsigned int SCR_HEIGHT = 1080 * 2;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; 
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
glm::vec3 secondPos(0.0f, 1.25f, 0.0f);
glm::vec3 thirdPos(0.0f, 2.5f, 0.0f);

struct Resources {
    Shader lightingShader;
    Camera camera;
    std::vector<glm::vec3> colors;
    unsigned int VBO;
    unsigned int cubeVAO;
    unsigned int width, height;
    int last_mouse_x, last_mouse_y;
};

struct Input {
    bool walk_left = false,
    walk_right = false,
    walk_up = false,
    walk_down = false,
    walk_forward = false,
    walk_backward = false,
    look_left = false,
    look_right = false,
    look_up = false,
    look_down = false,
    mouse_click_left = false;
    int mouse_motion_x = -1;
    int mouse_motion_y = -1;
};

void drawCube(Resources& res, glm::vec3 pos, glm::vec3 color) {
    auto model = glm::mat4();
    model = glm::translate(model, pos);
    res.lightingShader.setMat4("model", model);
    res.lightingShader.setVec3("objectColor", color.x, color.y, color.z);

    // render the second cube
    glBindVertexArray(res.cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Resources load_resources() {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/snes3.png").c_str(), &width, &height, &nrChannels, 0);
    std::vector<glm::vec3> colors{width * height};
    for (int i = 0; i < width * height; i++) {
        colors.push_back(glm::vec3{0.0f, 0.0f, 0.0f});
    }
    if (data)
    {
        std::cout << "width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        int index = 0;
        for (int j = height - 1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                if (nrChannels == 4 && data[index + 3] == 0) {
                    colors[j * width + i] = glm::vec3{0.5f, 0.5f, 0.5f};
                } else {
                    colors[j * width + i] = glm::vec3{
                        ((float) data[index + 0]) / 255.0f, 
                        ((float) data[index + 1]) / 255.0f,
                        ((float) data[index + 2]) / 255.0f
                    };
                }
                index += nrChannels;
            }
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    std::cout << "Colors OK" << std::endl;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Camera camera{glm::vec3{128.0f, 112.0f, 270.0f}};
    camera.MovementSpeed *= 10;

    return Resources {
        Shader{"resources/shaders/vertex.glsl", "resources/shaders/frags.glsl"},
        std::move(camera),
        std::move(colors),
        VBO,
        cubeVAO,
        width,
        height,
        -1,
        -1
    };
}

void update(const Input& input, Resources& res) {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float deltaTime = 1.0f / 30.0f;

    if (input.walk_up)
        res.camera.ProcessKeyboard(UP, deltaTime);
    if (input.walk_down)
        res.camera.ProcessKeyboard(DOWN, deltaTime);
    if (input.walk_forward)
        res.camera.ProcessKeyboard(FORWARD, deltaTime);
    if (input.walk_backward)
        res.camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (input.walk_left)
        res.camera.ProcessKeyboard(LEFT, deltaTime);
    if (input.walk_right)
        res.camera.ProcessKeyboard(RIGHT, deltaTime);

    if (input.mouse_click_left) {
        if (res.last_mouse_x < 0) {
            res.last_mouse_x = input.mouse_motion_x;
            res.last_mouse_y = input.mouse_motion_y;
        }
        float xoffset = input.mouse_motion_x - res.last_mouse_x;
        float yoffset = res.last_mouse_y - input.mouse_motion_y; // reversed since y-coordinates go from bottom to top

        res.last_mouse_x = input.mouse_motion_x;
        res.last_mouse_y = input.mouse_motion_y;

        res.camera.ProcessMouseMovement(xoffset, yoffset);
    } else {
        res.last_mouse_x = -1;
        res.last_mouse_y = -1;
    }

        // be sure to activate shader when setting uniforms/drawing objects
    res.lightingShader.use();
    res.lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    res.lightingShader.setVec3("lightPos", lightPos);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(res.camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = res.camera.GetViewMatrix();
    res.lightingShader.setMat4("projection", projection);
    res.lightingShader.setMat4("view", view);

    // world transformation

    float gap = 1.05f;

    int index = 0;
    for (int y = 0; y < res.height; y++) {
        for (int x = 0; x < res.width; x++) {
            drawCube(res, glm::vec3{x * gap, y * gap, 0.0f}, res.colors[index]);
            index ++;
        }
    }
}

int main()
{
// Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to init SDL\n";
        return -1;
    }

    atexit (SDL_Quit);

    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    auto main_window = SDL_CreateWindow(
        "Retro Drawer", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCR_WIDTH,
        SCR_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
    );

    if (main_window == nullptr) {
        std::cerr << "Failed to create SDL Window\n";
        return -1;
    }

    auto glContext = SDL_GL_CreateContext(main_window);
    if (glContext == nullptr) {
        std::cerr << "Failed to create OpenGL Context\n";
        return -1;
    }

    if (gladLoadGLLoader(SDL_GL_GetProcAddress) < 0) {
        std::cerr << "Failed to load OpenGL\n";
        return -1;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cerr << "Failed to set swap interval\n";
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    Input input;
    Resources res = load_resources();

    bool loop = true;
    while (loop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    loop = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            loop = false;
                            break;
                        case SDLK_a: input.walk_left = true; break;
                        case SDLK_d: input.walk_right = true; break;
                        case SDLK_w: input.walk_forward = true; break;
                        case SDLK_s: input.walk_backward = true; break;
                        case SDLK_q: input.walk_up = true; break;
                        case SDLK_e: input.walk_down = true; break;
                        case SDLK_UP: input.look_left = true; break;
                        case SDLK_DOWN: input.look_right = true; break;
                        case SDLK_LEFT: input.look_up = true; break;
                        case SDLK_RIGHT: input.look_down = true; break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_a: input.walk_left = false; break;
                        case SDLK_d: input.walk_right = false; break;
                        case SDLK_w: input.walk_forward = false; break;
                        case SDLK_s: input.walk_backward = false; break;
                        case SDLK_q: input.walk_up = false; break;
                        case SDLK_e: input.walk_down = false; break;
                        case SDLK_UP: input.look_left = false; break;
                        case SDLK_DOWN: input.look_right = false; break;
                        case SDLK_LEFT: input.look_up = false; break;
                        case SDLK_RIGHT: input.look_down = false; break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button)
                    {
                        case SDL_BUTTON_LEFT: input.mouse_click_left = true; break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button)
                    {
                        case SDL_BUTTON_LEFT: input.mouse_click_left = false; break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (input.mouse_click_left) {
                        input.mouse_motion_x = event.motion.x;
                        input.mouse_motion_y = event.motion.y;
                    } else {
                        input.mouse_motion_x = -1;
                        input.mouse_motion_y = -1;
                    }
                    break;
            }
        }

        update(input, res);

        SDL_GL_SwapWindow(main_window); 
    }


    return 0;
    /*
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation

        float gap = 2.5f;

        int index = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                drawCube(res, glm::vec3{x * gap, y * gap, 0.0f}, colors[index]);
                index ++;
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();*/
    return 0;
}
/*
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}*/