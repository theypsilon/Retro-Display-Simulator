#include <cstdio>

#include <glad/glad.h>

#include <SDL.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <vector>
#include <iostream>

#ifdef WIN32
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

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
    SDL_Window* window;
    unsigned int ticks;
    Uint32 last_time;
    Shader lightingShader;
    Camera camera;
    std::vector<glm::vec4> colors;
    unsigned int VBO;
    unsigned int cubeVAO;
    int width, height;
    int last_mouse_x, last_mouse_y;
    float cur_voxel_gap;
    float min_voxel_gap;
    bool full_screen;
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
    spread_voxels = false,
    collapse_voxels = false,
    mouse_click_left = false,
    f11 = false,
    alt = false,
    enter = false,
    speed_up = false,
    speed_down = false;
    int mouse_motion_x = -1;
    int mouse_motion_y = -1;
};

void drawCube(Resources& res, glm::vec3 pos, glm::vec4 color) {
    if (color.a == 0.0f) return;
    auto model = glm::mat4();
    model = glm::translate(model, pos);
    res.lightingShader.setMat4("model", model);
    res.lightingShader.setVec3("objectColor", color.x, color.y, color.z);

    // render the second cube
    glBindVertexArray(res.cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Resources load_resources(SDL_Window* window) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/snes3.png").c_str(), &width, &height, &nrChannels, 0);
    std::vector<glm::vec4> colors{(unsigned int) width * height};
    for (int i = 0; i < width * height; i++) {
        colors.push_back(glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});
    }
    if (data)
    {
        std::cout << "width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        int index = 0;
        for (int j = height - 1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                colors[j * width + i] = glm::vec4{
                    ((float) data[index + 0]) / 255.0f, 
                    ((float) data[index + 1]) / 255.0f,
                    ((float) data[index + 2]) / 255.0f,
                    ((float) data[index + 3]) / 255.0f
                };
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
        -0.5f * 1.16666f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f * 1.16666f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f * 1.16666f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f * 1.16666f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f * 1.16666f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f * 1.16666f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f * 1.16666f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f * 1.16666f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f * 1.16666f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f * 1.16666f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f * 1.16666f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f * 1.16666f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f * 1.16666f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f * 1.16666f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f * 1.16666f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f * 1.16666f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f * 1.16666f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f * 1.16666f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f * 1.16666f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f * 1.16666f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f * 1.16666f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f * 1.16666f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f * 1.16666f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f * 1.16666f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f * 1.16666f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f * 1.16666f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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

    Camera camera{glm::vec3{0.0f, 0.0f, 270.0f}};

    return Resources {
        window,
        0,
        0,
        Shader{"resources/shaders/vertex.glsl", "resources/shaders/frags.glsl"},
        std::move(camera),
        std::move(colors),
        VBO,
        cubeVAO,
        width,
        height,
        -1,
        -1,
        1.0f,
        1.0f,
        false
    };
}

void update(const Input& input, Resources& res) {
    if (input.f11) {
        auto flag = res.full_screen ? 0 : SDL_WINDOW_FULLSCREEN;
        SDL_SetWindowFullscreen(res.window, flag);
        res.full_screen = !res.full_screen;
    }

    auto now = SDL_GetTicks();
    const auto fps_time = 1000;
    if (now > res.last_time + fps_time) {
        std::cout << "FPS: " << (res.ticks / (fps_time / 1000)) << std::endl;
        res.last_time = now;
        res.ticks = 0;
    }
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float deltaTime = 1.0f / 30.0f;

    if (input.speed_up)
        res.camera.MovementSpeed *= 1.5f;
    if (input.speed_down)
        res.camera.MovementSpeed /= 1.5f;

    if (res.camera.MovementSpeed > 10000)
        res.camera.MovementSpeed = 10000;
    if (res.camera.MovementSpeed < 0.1)
        res.camera.MovementSpeed = 0.1f;

    if (input.spread_voxels)
        res.cur_voxel_gap += 0.5f * deltaTime;
    if (input.collapse_voxels)
        res.cur_voxel_gap -= 0.5f * deltaTime;
    if (res.cur_voxel_gap <= res.min_voxel_gap)
        res.cur_voxel_gap = res.min_voxel_gap;

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

    auto width = res.width;
    auto height = res.height;

    float gap = res.cur_voxel_gap;

    float half_width = width / 2 * gap * 1.16666f;
    float half_height = height / 2 * gap;
        // be sure to activate shader when setting uniforms/drawing objects
    res.lightingShader.use();
    res.lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    res.lightingShader.setVec3("lightPos", glm::vec3{half_width, half_height, 400.0f});

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(res.camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = res.camera.GetViewMatrix();
    res.lightingShader.setMat4("projection", projection);
    res.lightingShader.setMat4("view", view);

    // world transformation

    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            drawCube(res, glm::vec3{(float) x * gap * 1.16666f - half_width, (float) y * gap - half_height, 0.0f}, res.colors[index]);
            index ++;
        }
    }

    res.ticks++;
}

int main(int argc, char* argv[])
{
// Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to init SDL\n";
        return -1;
    }

    atexit (SDL_Quit);

    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    auto main_window = SDL_CreateWindow(
        "Retro Drawer", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCR_WIDTH,
        SCR_HEIGHT,
        SDL_WINDOW_OPENGL
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

	/*SDL_Renderer *renderer = SDL_CreateRenderer(main_window, -1, 0);
	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(renderer, &renderer_info);

	std::cout << "vendor: " << SDL_GetCurrentVideoDriver << "\nrenderer" << renderer_info.name << std::endl;
	*/
	/* we can now get data for the specific OpenGL instance we created */
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string) : %s\n", glGetString(GL_VERSION));
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cerr << "Failed to set swap interval\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Input input;
    Resources res = load_resources(main_window);

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
                        case SDLK_r: input.speed_down = true; break;
                        case SDLK_f: input.speed_up = true; break;
                        case SDLK_j: input.spread_voxels = true; break;
                        case SDLK_k: input.collapse_voxels = true; break;
						case SDLK_UP: input.walk_forward = true; break;
						case SDLK_DOWN: input.walk_backward = true; break;
						case SDLK_LEFT: input.walk_left = true; break;
						case SDLK_RIGHT: input.walk_right = true; break;
                        case SDLK_F11: input.f11 = true; break;
                        case SDLK_LALT: input.alt = true; break;
                        case SDLK_RETURN: input.enter = true; break;
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

		const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
		if (kbstate[SDL_SCANCODE_A     ] == false && kbstate[SDL_SCANCODE_LEFT ] == false && input.walk_left       == true) { input.walk_left       = false; }
		if (kbstate[SDL_SCANCODE_D     ] == false && kbstate[SDL_SCANCODE_RIGHT] == false && input.walk_right      == true) { input.walk_right      = false; }
		if (kbstate[SDL_SCANCODE_W     ] == false && kbstate[SDL_SCANCODE_UP   ] == false && input.walk_forward    == true) { input.walk_forward    = false; }
		if (kbstate[SDL_SCANCODE_S     ] == false && kbstate[SDL_SCANCODE_DOWN ] == false && input.walk_backward   == true) { input.walk_backward   = false; }
		if (kbstate[SDL_SCANCODE_Q     ] == false && input.walk_up         == true) { input.walk_up         = false; }
		if (kbstate[SDL_SCANCODE_E     ] == false && input.walk_down       == true) { input.walk_down       = false; }
		if (kbstate[SDL_SCANCODE_J     ] == false && input.spread_voxels   == true) { input.spread_voxels   = false; }
		if (kbstate[SDL_SCANCODE_K     ] == false && input.collapse_voxels == true) { input.collapse_voxels = false; }
		if (kbstate[SDL_SCANCODE_F     ] == false && input.speed_up        == true) { input.speed_up        = false; }
		if (kbstate[SDL_SCANCODE_R     ] == false && input.speed_down      == true) { input.speed_down      = false; }
        if (kbstate[SDL_SCANCODE_RETURN] == false && input.enter       == true) { input.enter       = false; }
        if (kbstate[SDL_SCANCODE_LALT  ] == false && input.alt       == true) { input.alt       = false; }

        if (input.alt && input.enter) {
            input.alt = false;
            input.enter = false;
            input.f11 = true;
        }

        update(input, res);

        if (input.f11) { input.f11 = false; }
        if (input.speed_down) { input.speed_down = false; }
        if (input.speed_up) { input.speed_up = false; }

        SDL_GL_SwapWindow(main_window); 
    }


    return 0;
    return 0;
}
