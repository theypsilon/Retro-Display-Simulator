#include <glad/glad.h>

#include <SDL.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <theypsilon/boolean_button.h>

#include <cstdio>
#include <cmath>
#include <vector>
#include <iostream>
#include <chrono>

#ifdef WIN32
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

struct InternalButtons {
	ty::boolean_button speed_up, speed_down, f1, f11, lalt, enter;
};

struct Resources {
    SDL_Window* window;
    unsigned int ticks;
    Uint32 last_time;
    Shader lightingShader;
    Camera camera;
    unsigned int cubeVAO;
    int width, height;
    int last_mouse_x, last_mouse_y;
    float cur_voxel_gap;
    float min_voxel_gap;
    bool full_screen;
	float wave;
	GLuint info_texture;
	unsigned int infoVAO;
	Shader infoShader;
    bool showing_info;
	InternalButtons buttons;
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
    f1 = false,
    f11 = false,
    alt = false,
    enter = false,
    speed_up = false,
    speed_down = false;
    int mouse_motion_x = -1;
    int mouse_motion_y = -1;
};


const long double ratio_4_3 = 4.0 / 3.0;
const long double ratio_256_224 = 256.0 / 224.0;
const long double snes_factor_horizontal = 1.1666666666666666666666666667; //ratio_4_3 / ratio_256_224;
const long double snes_factor_vertical = 1.0; //1.0 / snes_factor_horizontal;

unsigned int SCR_WIDTH  = 1920 *2;
unsigned int SCR_HEIGHT = 1080 *2;

Resources load_resources(SDL_Window* window, const std::string& path);
void update(const Input& input, Resources& res, float delta_time);
void read_input(Input& input, bool& loop);
void windows_high_dpi_hack();

int main(int argc, char* argv[]) {
	std::cout << __func__ << std::endl;
    const auto path = FileSystem::getPath(argc > 1 ? std::string{argv[1]} : "resources/textures/snes3.png");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to init SDL\n";
        return -1;
    }

    atexit (SDL_Quit);

#ifdef WIN32
	windows_high_dpi_hack();
#else
    SDL_DisplayMode display_mode;
    if (SDL_GetDesktopDisplayMode(0, &display_mode) == 0) {
		std::cout << "Resolution: " << display_mode.w << "x" << display_mode.h << std::endl;
        SCR_WIDTH = display_mode.w;
        SCR_HEIGHT = display_mode.h;
    }
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // It doesn't work combined with MSAA, I don't know why.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // This is MSAA on/off
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // This is MSAA number of sampling

    auto main_window = SDL_CreateWindow(
        "Retro Voxel Display", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCR_WIDTH,
        SCR_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
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

    glEnable(GL_MULTISAMPLE);  
    glEnable(GL_DEPTH_TEST);

    Input input;
    Resources res = load_resources(main_window, path);

    bool loop = true;
    float delta_time = 0.0f;
    auto last_time = std::chrono::system_clock::now();
    while (loop) {
        auto current_time = std::chrono::system_clock::now();
        delta_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time).count() / 1000000.0f;
        last_time = current_time;

        read_input(input, loop);
        update(input, res, delta_time);

        SDL_GL_SwapWindow(main_window); 
    }

    return 0;
}


Resources load_resources(SDL_Window* window, const std::string& path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    std::cout << "Will load instances: " << (width * height) << ", Size: " << (width * height * 16) << std::endl;
    std::vector<glm::vec4> colors(width * height);
    if (data) {
        std::cout << "width: " << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;
        int index = 0;
        for (int j = 0; j < height; j++) {
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
		stbi_image_free(data);
    } else {
        std::cout << "Failed to load target texture" << std::endl;
    }
	int info_width, info_height, info_nr_channels;
	GLuint info_texture;
	data = stbi_load(FileSystem::getPath("resources/textures/info.png").c_str(), &info_width, &info_height, &info_nr_channels, 0);
	unsigned int infoVAO;
	if (data) {
		float vertices[] = {
			// positions          // colors           // texture coords
			1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			1.0f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			0.75f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			0.75f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		unsigned int VBO, EBO;
        glGenVertexArrays(1, &infoVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

		glBindVertexArray(infoVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenTextures(1, &info_texture);

		glBindTexture(GL_TEXTURE_2D, info_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info_width, info_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	} else {
		std::cout << "Failed to load info texture" << std::endl;
	}
    float gap = 1.0f;
    float half_width = float(width) / 2.0f * gap * snes_factor_horizontal;
    float half_height = float(height) / 2.0f * gap;

    std::vector<glm::vec2> offsets(width * height);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            float x = i * gap * snes_factor_horizontal - half_width;
            float y = j * gap - half_height;
            offsets[j * width + i] = glm::vec2(x, y);
        }
    }

    float vertices[] = {
        // cube coordinates                                           cube normals
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f,  0.0f, -1.0f,

        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,

        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      -1.0f,  0.0f,  0.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      -1.0f,  0.0f,  0.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      -1.0f,  0.0f,  0.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      -1.0f,  0.0f,  0.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      -1.0f,  0.0f,  0.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      -1.0f,  0.0f,  0.0f,

         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      1.0f,  0.0f,  0.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      1.0f,  0.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      1.0f,  0.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      1.0f,  0.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      1.0f,  0.0f,  0.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      1.0f,  0.0f,  0.0f,

        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f, -1.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f, -1.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f, -1.0f,  0.0f,
         0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f, -1.0f,  0.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f, -1.0f,  0.0f,
        -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical, -0.5f,      0.0f, -1.0f,  0.0f,

        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  1.0f,  0.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  1.0f,  0.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  1.0f,  0.0f,
         0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  1.0f,  0.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  1.0f,  0.0f,
        -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  1.0f,  0.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int geometryVBO;
    glGenBuffers(1, &geometryVBO);
    glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int colorsVBO;
    glGenBuffers(1, &colorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * width * height, colors.data(), GL_STATIC_DRAW);

    // color attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(2, 1);


    unsigned int offsetsVBO;
    glGenBuffers(1, &offsetsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * width * height, offsets.data(), GL_STATIC_DRAW);

    // offset attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);

    // be sure to activate shader when setting uniforms/drawing objects
    Shader lightingShader{
        FileSystem::getPath("resources/shaders/vertex.glsl").c_str(),
        FileSystem::getPath("resources/shaders/frags.glsl").c_str()
    };
	Shader infoShader{
		FileSystem::getPath("resources/shaders/info_vertex.glsl").c_str(),
		FileSystem::getPath("resources/shaders/info_frags.glsl").c_str()
	};
    Camera camera{glm::vec3{0.0f, 0.0f, 270.0f}};
	camera.MovementSpeed *= 5;
    return Resources {
        window,
        0,
        0,
        std::move(lightingShader),
        std::move(camera),
        VAO,
        width,
        height,
        -1,
        -1,
        1.0f,
        1.0f,
        false,
		0.0f,
		info_texture,
		infoVAO,
		std::move(infoShader),
        true
    };
}

void update(const Input& input, Resources& res, float delta_time) {
	res.buttons.f11.track(input.f11);
	res.buttons.lalt.track(input.alt);
	res.buttons.enter.track(input.enter);
    if (res.buttons.f11.just_pressed() || res.buttons.enter.just_pressed() && res.buttons.lalt || res.buttons.enter && res.buttons.lalt.just_pressed()) {
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

	res.buttons.f1.track(input.f1);
	if (res.buttons.f1.just_pressed()) {
		res.showing_info = !res.showing_info;
	}

	res.buttons.speed_up.track(input.speed_up);
    if (res.buttons.speed_up.just_pressed())
        res.camera.MovementSpeed *= 1.5f;

	res.buttons.speed_down.track(input.speed_down);
    if (res.buttons.speed_down.just_pressed())
        res.camera.MovementSpeed /= 1.5f;

    if (res.camera.MovementSpeed > 10000)
        res.camera.MovementSpeed = 10000;
    if (res.camera.MovementSpeed < 0.1)
        res.camera.MovementSpeed = 0.1f;

    if (input.spread_voxels)
        res.cur_voxel_gap += 0.005f * delta_time * res.camera.MovementSpeed;
    if (input.collapse_voxels)
        res.cur_voxel_gap -= 0.005f * delta_time * res.camera.MovementSpeed;
    if (res.cur_voxel_gap <= res.min_voxel_gap)
        res.cur_voxel_gap = res.min_voxel_gap;

    if (input.walk_up)
        res.camera.ProcessKeyboard(UP, delta_time);
    if (input.walk_down)
        res.camera.ProcessKeyboard(DOWN, delta_time);
    if (input.walk_forward)
        res.camera.ProcessKeyboard(FORWARD, delta_time);
    if (input.walk_backward)
        res.camera.ProcessKeyboard(BACKWARD, delta_time);
    if (input.walk_left)
        res.camera.ProcessKeyboard(LEFT, delta_time);
    if (input.walk_right)
        res.camera.ProcessKeyboard(RIGHT, delta_time);

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

    float gap_value = res.cur_voxel_gap;

    glm::mat4 projection = glm::perspective(glm::radians(res.camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = res.camera.GetViewMatrix();

    res.lightingShader.use();
	res.lightingShader.setFloat("wave", res.wave * 7);
    res.lightingShader.setFloat("ambientStrength", 0.5f);
    res.lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    res.lightingShader.setVec3("lightPos", glm::vec3{width / 2, height / 2, 400.0f});
    res.lightingShader.setMat4("projection", projection);
    res.lightingShader.setMat4("view", view);
    res.lightingShader.setVec2("gap", glm::vec2{gap_value, gap_value});

	res.wave += delta_time * 0.1f;

    // world transformation
    glBindVertexArray(res.cubeVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, res.width * res.height);

    if (res.showing_info) {
        glBindTexture(GL_TEXTURE_2D, res.info_texture);
        res.infoShader.use();
        res.infoShader.setFloat("wave", res.wave);
        glBindVertexArray(res.infoVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    res.ticks++;
}

void read_input(Input& input, bool& loop) {
	SDL_PumpEvents();

	input.mouse_click_left = SDL_GetMouseState(&input.mouse_motion_x, &input.mouse_motion_y) & SDL_BUTTON(SDL_BUTTON_LEFT);

    const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
	loop = kbstate[SDL_SCANCODE_ESCAPE] == false && SDL_QuitRequested() == false;
    input.walk_left       = kbstate[SDL_SCANCODE_A     ] || kbstate[SDL_SCANCODE_LEFT ];
    input.walk_right      = kbstate[SDL_SCANCODE_D     ] || kbstate[SDL_SCANCODE_RIGHT];
    input.walk_forward    = kbstate[SDL_SCANCODE_W     ] || kbstate[SDL_SCANCODE_UP   ];
    input.walk_backward   = kbstate[SDL_SCANCODE_S     ] || kbstate[SDL_SCANCODE_DOWN ];
    input.walk_up         = kbstate[SDL_SCANCODE_Q     ];
    input.walk_down       = kbstate[SDL_SCANCODE_E     ];
    input.spread_voxels   = kbstate[SDL_SCANCODE_J     ];
    input.collapse_voxels = kbstate[SDL_SCANCODE_K     ];
	input.speed_up        = kbstate[SDL_SCANCODE_F];
	input.speed_down      = kbstate[SDL_SCANCODE_R];
	input.f1              = kbstate[SDL_SCANCODE_F1];
	input.f11             = kbstate[SDL_SCANCODE_F11];
	input.alt = kbstate[SDL_SCANCODE_LALT];
	input.enter = kbstate[SDL_SCANCODE_RETURN];
}

void windows_high_dpi_hack() {
#ifdef WIN32
	typedef enum PROCESS_DPI_AWARENESS {
		PROCESS_DPI_UNAWARE = 0,
		PROCESS_SYSTEM_DPI_AWARE = 1,
		PROCESS_PER_MONITOR_DPI_AWARE = 2
	} PROCESS_DPI_AWARENESS;

	void* userDLL;
	BOOL(WINAPI *SetProcessDPIAware)(void); // Vista and later
	void* shcoreDLL;
	HRESULT(WINAPI *SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness); // Windows 8.1 and later

	userDLL = SDL_LoadObject("USER32.DLL");
	if (userDLL) {
		SetProcessDPIAware = (BOOL(WINAPI *)(void)) SDL_LoadFunction(userDLL, "SetProcessDPIAware");
	}

	shcoreDLL = SDL_LoadObject("SHCORE.DLL");
	if (shcoreDLL) {
		SetProcessDpiAwareness = (HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS)) SDL_LoadFunction(shcoreDLL, "SetProcessDpiAwareness");
	}

	if (SetProcessDpiAwareness) {
		/* Try Windows 8.1+ version */
		HRESULT result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		SDL_Log("called SetProcessDpiAwareness: %d", (result == S_OK) ? 1 : 0);
	}
	else if (SetProcessDPIAware) {
		/* Try Vista - Windows 8 version.
		This has a constant scale factor for all monitors.
		*/
		BOOL success = SetProcessDPIAware();
		SDL_Log("called SetProcessDPIAware: %d", (int)success);
	}
#endif
}