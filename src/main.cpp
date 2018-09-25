#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "resources.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <theypsilon/camera.h>
#include <theypsilon/boolean_button.h>
#include <theypsilon/error.h>

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

#ifdef WIN32
const bool on_windows = true;
#else
const bool on_windows = false;
#endif

#if defined (_MSC_VER)
#define ATTR_UNUSED
#else
#define ATTR_UNUSED __attribute__((unused))
#endif

struct InternalButtons {
	ty::boolean_button speed_up, speed_down, f1, f11, lalt, enter, waving, swap_voxels_to_pixels, mouse_click;
};

struct AnimationPaths {
    std::vector<const char*> paths;
    int milliseconds = 100;
};

struct AnimationColors {
	std::vector<std::vector<glm::vec4>> colors_by_image;
	int milliseconds;
};

struct InfoResources {
	GLuint info_texture;
	unsigned int info_vao;
	Shader info_shader;
	float info_mixer;
	bool showing_info;
};

struct Screen {
	GLFWwindow* window;
	int width, height, refresh_rate;
};

struct Resources {
    Screen screen;
    unsigned int ticks;
    std::chrono::time_point<std::chrono::high_resolution_clock> now;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_time;
    Shader lighting_shader;
	InfoResources info_panel;
	ty::Camera camera;
	double camera_zoom;
    unsigned int voxel_vao;
    unsigned int voxel_vbo;
    unsigned int colors_vbo;
	AnimationColors animation_colors;
    int image_width, image_height;
	unsigned int image_counter;
	std::chrono::time_point<std::chrono::high_resolution_clock> image_tick;
    double last_mouse_x, last_mouse_y;
    double cur_voxel_scale_x;
	double cur_voxel_scale_y;
	double cur_voxel_gap;
	float voxels_pulse;
	bool showing_waves;
    bool showing_voxels;
	InternalButtons buttons;
};

struct Input {
	bool escape = false,
		space = false,
        walk_left = false,
		walk_right = false,
		walk_up = false,
		walk_down = false,
		walk_forward = false,
		walk_backward = false,
		look_left = false,
		look_right = false,
		look_up = false,
		look_down = false,
		turn_up = false,
		turn_down = false,
		turn_left = false,
		turn_right = false,
        rotate_left = false,
        rotate_right = false,
        swap_voxels_to_pixels = false,
		increase_voxel_scale_y = false,
		decrease_voxel_scale_y = false,
		increase_voxel_scale_x = false,
		decrease_voxel_scale_x = false,
        increase_voxel_gap = false,
        decrease_voxel_gap = false,
		mouse_click_left = false,
		f1 = false,
		f11 = false,
		alt = false,
		enter = false,
		speed_up = false,
		speed_down = false,
		change_view = false,
		change_waving = false;
    double mouse_motion_x = -1;
    double mouse_motion_y = -1;
	double mouse_scroll_y = 0;
};

const long double ratio_4_3 = 4.0 / 3.0;
const long double ratio_256_224 = 256.0 / 224.0;

const double snes_factor_horizontal = ratio_4_3 / ratio_256_224;

const float cube_geometry[] = {
    // cube coordinates       cube normals
    -0.5f, -0.5f, -0.5f,      0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,      0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,      0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,      0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,      0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,      0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,      -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,      -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,      1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,      1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,      1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,      1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,      0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,      0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,      0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,      0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,      0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,      0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,      0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,      0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,      0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,      0.0f,  1.0f,  0.0f,
};

const float square_geometry[] = {
    -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
};

ty::error program(int argc, char* argv[]);
ty::result<Resources> load_resources(Screen screen, const AnimationPaths& animation_paths);
ty::result<InfoResources> load_info_resources();
ty::result<std::vector<std::vector<glm::vec4>>> load_animation(const std::vector<const char*>& paths, int& image_width, int& image_height);
ty::error load_image_on_gpu(const std::vector<glm::vec4>& colors, const int image_width, const int image_height, const unsigned int colors_vbo);
ty::error update(const Input& input, Resources& res, float delta_time);

const AnimationPaths animation_collection[] = {
	AnimationPaths{{
		"resources/textures/wwix_00.png",
		"resources/textures/wwix_01.png",
		"resources/textures/wwix_02.png",
		"resources/textures/wwix_03.png",
		"resources/textures/wwix_04.png",
		"resources/textures/wwix_05.png",
		"resources/textures/wwix_06.png",
		"resources/textures/wwix_07.png",
		"resources/textures/wwix_08.png",
		"resources/textures/wwix_09.png",
		"resources/textures/wwix_10.png",
		"resources/textures/wwix_11.png",
		"resources/textures/wwix_12.png",
		"resources/textures/wwix_13.png",
		"resources/textures/wwix_14.png",
		"resources/textures/wwix_15.png",
		"resources/textures/wwix_16.png",
		"resources/textures/wwix_17.png",
		"resources/textures/wwix_18.png",
		"resources/textures/wwix_19.png",
		"resources/textures/wwix_20.png",
		"resources/textures/wwix_21.png",
		"resources/textures/wwix_22.png",
		"resources/textures/wwix_23.png",
		"resources/textures/wwix_24.png",
		"resources/textures/wwix_25.png",
		"resources/textures/wwix_26.png",
		"resources/textures/wwix_27.png",
		"resources/textures/wwix_28.png",
		"resources/textures/wwix_29.png",
		"resources/textures/wwix_30.png",
		"resources/textures/wwix_31.png",
		"resources/textures/wwix_32.png",
		"resources/textures/wwix_33.png",
		"resources/textures/wwix_34.png",
		"resources/textures/wwix_35.png",
		"resources/textures/wwix_36.png",
		"resources/textures/wwix_37.png",
		"resources/textures/wwix_38.png",
		"resources/textures/wwix_39.png",
		"resources/textures/wwix_40.png",
		"resources/textures/wwix_41.png",
		"resources/textures/wwix_42.png",
		"resources/textures/wwix_43.png",
		"resources/textures/wwix_44.png",
		"resources/textures/wwix_45.png",
	}, 16}
};

auto animation_collection_size = sizeof(animation_collection) / sizeof(animation_collection[0]);

int main(int argc, char* argv[]) {
	auto err = program(argc, argv);
	if (err) {
		std::cerr << "Ooops! Something went wrong!\n[ERROR] " << err.message() << "\nClosing program in 10 seconds.\n";
		using namespace std::literals;
		std::this_thread::sleep_for(10s);
		return -1;
	}
	return 0;
}

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "x.x.x"
#define PROJECT_BINARY_NAME "noname"
#define PROJECT_OFFICIAL_NAME "No Name Project"
#endif

ty::error program(int argc, char* argv[]) {
#ifdef DEBUG
	std::cout << "DEBUG on!" << std::endl;
#endif
	std::cout << "Starting " << PROJECT_OFFICIAL_NAME << " " << PROJECT_VERSION << std::endl;
	std::srand((unsigned int)std::time(nullptr));	
	auto animation_paths = animation_collection[std::rand() % animation_collection_size];
	if (argc > 1) {
		animation_paths = AnimationPaths{ { argv[1] } };
	}
	std::cout << "Playing image '" << animation_paths.paths[0] << "'.\n";

	TRY_IS_TRUE(glfwInit());
	atexit(glfwTerminate);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DOUBLEBUFFER, 2);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(SDL_GL_ACCELERATED_VISUAL, 1); // I don't know what is the proper alternative in GLFW.

	glfwWindowHint(GLFW_SAMPLES, 4);

	TRY_NOT_NULL(auto, video_mode, glfwGetVideoMode(glfwGetPrimaryMonitor()));
	Screen screen;
	screen.width = video_mode->width;
	screen.height = video_mode->height;
	screen.refresh_rate = video_mode->refreshRate;

	std::cout << "Creating window with resolution " << screen.width << "x" << screen.height << "\n";
	std::cout << "Max refresh rate " << screen.refresh_rate << "hz.\n";

	TRY_NOT_NULL(, screen.window, glfwCreateWindow(screen.width, screen.height, PROJECT_OFFICIAL_NAME, on_windows ? nullptr : glfwGetPrimaryMonitor(), nullptr));
	glfwSetWindowPos(screen.window, 0, 0);

	glfwMakeContextCurrent(screen.window);
	TRY_NON_NEG(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string) : %s\n", glGetString(GL_VERSION));
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSwapInterval(0);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	Input input;
	
	TRY_RESULT(auto, res, load_resources(screen, animation_paths));

	glfwSetWindowUserPointer(res.screen.window, &input);
	glfwSetCursorPosCallback(res.screen.window, [](GLFWwindow* window, double xpos, double ypos) {
		auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
		input.mouse_motion_x = xpos;
		input.mouse_motion_y = ypos;
	});
	glfwSetMouseButtonCallback(res.screen.window, [](GLFWwindow* window, int key, int action, ATTR_UNUSED int mods) {
		if (key != GLFW_MOUSE_BUTTON_LEFT || action == GLFW_REPEAT) return;

		auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
		input.mouse_click_left = action == GLFW_PRESS ? true : false;
	});
	glfwSetScrollCallback(res.screen.window, [](GLFWwindow* window, ATTR_UNUSED double xoffset, double yoffset) {
		auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
		input.mouse_scroll_y = yoffset;
	});
	glfwSetKeyCallback(res.screen.window, [](GLFWwindow* window, int key, ATTR_UNUSED int scancode, int action, ATTR_UNUSED int mods) {
		if (action == GLFW_REPEAT) return;

		auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));

        bool activation = action == GLFW_PRESS ? true : false;

        switch(key) {
            case (GLFW_KEY_ESCAPE)    :input.escape                   = activation; break;
			case (GLFW_KEY_SPACE)     :input.space                    = activation; break;
    			case (GLFW_KEY_A)         :input.walk_left                = activation; break;
    			case (GLFW_KEY_D)         :input.walk_right               = activation; break;
    			case (GLFW_KEY_W)         :input.walk_forward             = activation; break;
    			case (GLFW_KEY_S)         :input.walk_backward            = activation; break;
    			case (GLFW_KEY_Q)         :input.walk_up                  = activation; break;
    			case (GLFW_KEY_E)         :input.walk_down                = activation; break;
    			case (GLFW_KEY_LEFT)      :input.turn_left                = activation; break;
    			case (GLFW_KEY_RIGHT)     :input.turn_right               = activation; break;
    			case (GLFW_KEY_UP)        :input.turn_up                  = activation; break;
    			case (GLFW_KEY_DOWN)      :input.turn_down                = activation; break;
    			case (GLFW_KEY_O)         :input.swap_voxels_to_pixels    = activation; break;
    			case (GLFW_KEY_J)         :input.increase_voxel_scale_y   = activation; break;
    			case (GLFW_KEY_K)         :input.decrease_voxel_scale_y   = activation; break;
    			case (GLFW_KEY_U)         :input.increase_voxel_scale_x   = activation; break;
    			case (GLFW_KEY_I)         :input.decrease_voxel_scale_x   = activation; break;
    			case (GLFW_KEY_N)         :input.increase_voxel_gap       = activation; break;
    			case (GLFW_KEY_M)         :input.decrease_voxel_gap       = activation; break;
    			case (GLFW_KEY_F)         :input.speed_up                 = activation; break;
    			case (GLFW_KEY_R)         :input.speed_down               = activation; break;
    			case (GLFW_KEY_F1)        :input.f1                       = activation; break;
    			case (GLFW_KEY_F11)       :input.f11                      = activation; break;
    			case (GLFW_KEY_LEFT_ALT)  :input.alt                      = activation; break;
    			case (GLFW_KEY_ENTER)     :input.enter                    = activation; break;
    			case (GLFW_KEY_C)         :input.change_view              = activation; break;
    			case (GLFW_KEY_P)         :input.change_waving            = activation; break;
			case (GLFW_KEY_KP_ADD)	  :input.rotate_right             = activation; break;
			case (GLFW_KEY_KP_SUBTRACT)     :input.rotate_left        = activation; break;
        }
	});

	const int frame_length_us = 1000000 / screen.refresh_rate;
	auto last_time = res.now;
	while (input.escape == false && glfwWindowShouldClose(res.screen.window) == false) {
		res.now = std::chrono::high_resolution_clock::now();
		const auto time_since_last_frame_us = std::chrono::duration_cast<std::chrono::microseconds>(res.now - last_time).count();
		const long long int time_diff = frame_length_us - time_since_last_frame_us;
		if (time_diff > 0) continue;
		const auto delta_time = double(time_since_last_frame_us) / 1000000.0;
		last_time = res.now - std::chrono::microseconds(time_diff);

		TRY_ERROR(update(input, res, float(delta_time)));

		input.mouse_scroll_y = 0;

		glfwSwapBuffers(res.screen.window);
		glfwPollEvents();

		TRY_NOT_GL_ERROR();
	}

	RETURN_OK;
}

ty::result<Resources> load_resources(Screen screen, const AnimationPaths& animation_paths) {
    int image_width = 0, image_height = 0;

    unsigned int voxel_vao;
    glGenVertexArrays(1, &voxel_vao);
    glBindVertexArray(voxel_vao);

    unsigned int voxel_vbo;
    glGenBuffers(1, &voxel_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, voxel_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_geometry), square_geometry, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int colors_vbo;
    glGenBuffers(1, &colors_vbo);
    TRY_RESULT(auto, colors_by_image, load_animation(animation_paths.paths, image_width, image_height));
	TRY_ERROR(load_image_on_gpu(colors_by_image[0], image_width, image_height, colors_vbo));

    float voxel_scale = 1.0f;
    float half_width = float(image_width) / 2.0f * voxel_scale;
    float half_height = float(image_height) / 2.0f * voxel_scale;

    int offset_multiplier = 1;

    std::vector<glm::vec2> offsets(image_width * image_height);
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            float x = i * voxel_scale - half_width;
            float y = j * voxel_scale - half_height;
            offsets[j * image_width + i] = glm::vec2(x * offset_multiplier, y * offset_multiplier);
        }
    }

    // color attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(2, 1);


    unsigned int offsets_vbo;
    glGenBuffers(1, &offsets_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, offsets_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * image_width * image_height, offsets.data(), GL_STATIC_DRAW);

    // offset attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);

    // be sure to activate shader when setting uniforms/drawing objects
    TRY_RESULT(Shader, lighting_shader, Shader::load_shader(
        "resources/shaders/voxel.vs",
        "resources/shaders/voxel.fs"
    ));

	TRY_NOT_GL_ERROR();

	ty::Camera camera{};
	camera.movement_speed *= 5;
	camera.SetPosition(glm::vec3{ 0.0f, 0.0f, 270.0f * offset_multiplier });

	TRY_RESULT(auto, info_panel, load_info_resources());

	Resources res{};
    res.screen = screen;
    res.ticks = 0;
    res.now = std::chrono::high_resolution_clock::now();
    res.last_time = res.now;
    res.lighting_shader = std::move(lighting_shader);
	res.info_panel = std::move(info_panel);
	res.camera = std::move(camera);
	res.camera_zoom = 45.0f;
    res.voxel_vao = voxel_vao;
    res.voxel_vbo = voxel_vbo;
    res.colors_vbo = colors_vbo;
	res.animation_colors = AnimationColors{ std::move(colors_by_image), animation_paths.milliseconds };
    res.image_width = image_width;
    res.image_height = image_height;
    res.image_counter = 0;
    res.image_tick = res.now;
    res.last_mouse_x = -1;
    res.last_mouse_y = -1;
    res.cur_voxel_scale_x = 0.0f;
    res.cur_voxel_scale_y = 0.0f;
    res.cur_voxel_gap = 0.0f;
	res.voxels_pulse = 0.0f;
    res.showing_waves = false;
    res.showing_voxels = false;
    return res;
}

ty::result<InfoResources> load_info_resources() {
	InfoResources info_res;

	GLuint info_texture;
	TRY_RESULT(auto, image, Image_Data::load("resources/textures/info.png", 0));
	unsigned int info_vao;
	float vertices_info[] = {
		// positions          // colors           // texture coords
		1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		1.0f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		0.75f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		0.75f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices_info[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int info_vbo, info_ebo;
	glGenVertexArrays(1, &info_vao);
	glGenBuffers(1, &info_vbo);
	glGenBuffers(1, &info_ebo);

	glBindVertexArray(info_vao);

	glBindBuffer(GL_ARRAY_BUFFER, info_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_info), vertices_info, GL_STATIC_DRAW);

	TRY_NOT_GL_ERROR();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_info), indices_info, GL_STATIC_DRAW);

	TRY_NOT_GL_ERROR();

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get_data());
	glGenerateMipmap(GL_TEXTURE_2D);

	TRY_NOT_GL_ERROR();

	TRY_RESULT(Shader, info_shader, Shader::load_shader(
		"resources/shaders/info_panel.vs",
		"resources/shaders/info_panel.fs"
	));

	info_res.info_texture = info_texture;
	info_res.info_vao = info_vao;
	info_res.info_shader = std::move(info_shader);
	info_res.info_mixer = 0.0f;
	info_res.showing_info = true;
	return info_res;
}

ty::result<std::vector<std::vector<glm::vec4>>> load_animation(const std::vector<const char*>& paths, int& image_width, int& image_height) {
	std::vector<std::vector<glm::vec4>> colors_by_image;
	for (auto path : paths) {
		TRY_RESULT(auto, current_image, Image_Data::load(path, 0));
		if (image_width == 0 && image_height == 0) {
			image_width = current_image.width;
			image_height = current_image.height;
		} else if (image_width != current_image.width || image_height != current_image.height) {
			RETURN_ERROR("width and height from image '" + path + "' does not match with the first image of the animation");
		}
		std::vector<glm::vec4> colors(image_width * image_height);
		int index = 0;
		auto data = current_image.get_data();
		for (int j = 0; j < image_height; j++) {
			for (int i = 0; i < image_width; i++) {
				colors[j * image_width + i] = glm::vec4{
					((float)data[index + 0]) / 255.0f,
					((float)data[index + 1]) / 255.0f,
					((float)data[index + 2]) / 255.0f,
					current_image.nr_channels > 3 ? ((float)data[index + 3]) / 255.0f : 1.0f
				};
				index += current_image.nr_channels;
			}
		}
		colors_by_image.emplace_back(std::move(colors));
	}
	return colors_by_image;
}

ty::error load_image_on_gpu(const std::vector<glm::vec4>& colors, const int image_width, const int image_height, const unsigned int colors_vbo) {
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * image_width * image_height, colors.data(), GL_STATIC_DRAW);

	TRY_NOT_GL_ERROR();

	RETURN_OK;
}

ty::error update(const Input& input, Resources& res, float delta_time) {
    if (res.now > res.last_time + std::chrono::seconds(1)) {
        std::cout << "FPS: " << res.ticks << std::endl;
        res.last_time = res.now;
        res.ticks = 0;
    }
	res.ticks++;

    if (res.animation_colors.colors_by_image.size() > 1 && res.now > res.image_tick + std::chrono::milliseconds(res.animation_colors.milliseconds)) {
        res.image_tick = res.now;

        res.image_counter++;
        if (res.image_counter >= res.animation_colors.colors_by_image.size()) {
            res.image_counter = 0;
        }

        TRY_ERROR(load_image_on_gpu(res.animation_colors.colors_by_image[res.image_counter], res.image_width, res.image_height, res.colors_vbo));
    }

	res.buttons.waving.track(input.change_waving);
	if (res.buttons.waving.just_released()) {
		res.showing_waves = !res.showing_waves;
	}

    res.buttons.swap_voxels_to_pixels.track(input.swap_voxels_to_pixels);
    if (res.buttons.swap_voxels_to_pixels.just_released()) {
        glBindBuffer(GL_ARRAY_BUFFER, res.voxel_vbo);
        if (res.showing_voxels) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(square_geometry), square_geometry, GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, sizeof(cube_geometry), cube_geometry, GL_STATIC_DRAW);
        }

		TRY_NOT_GL_ERROR();

        std::cout << "Activating " << (res.showing_voxels ? "pixels" : "voxels") << ".\n";
        res.showing_voxels = !res.showing_voxels;
    }

	res.buttons.f1.track(input.f1);
	if (res.buttons.f1.just_pressed()) {
		res.info_panel.showing_info = !res.info_panel.showing_info;
	}

	res.buttons.speed_up.track(input.speed_up);
    if (res.buttons.speed_up.just_pressed()) {
        res.camera.movement_speed *= 1.5f; }

	res.buttons.speed_down.track(input.speed_down);
    if (res.buttons.speed_down.just_pressed()) {
        res.camera.movement_speed /= 1.5f; }

    if (res.camera.movement_speed > 10000) {
        res.camera.movement_speed = 10000; }
    if (res.camera.movement_speed < 0.1) {
        res.camera.movement_speed = 0.1f; }

    if (input.increase_voxel_scale_x) {
        res.cur_voxel_scale_x += 0.005 * delta_time * res.camera.movement_speed; }
    if (input.decrease_voxel_scale_x) {
        res.cur_voxel_scale_x -= 0.005 * delta_time * res.camera.movement_speed; }
    if (res.cur_voxel_scale_x <= 0) {
        res.cur_voxel_scale_x = 0; }

	if (input.increase_voxel_scale_y) {
		res.cur_voxel_scale_y += 0.005 * delta_time * res.camera.movement_speed; }
	if (input.decrease_voxel_scale_y) {
		res.cur_voxel_scale_y -= 0.005 * delta_time * res.camera.movement_speed; }
	if (res.cur_voxel_scale_y <= 0) {
		res.cur_voxel_scale_y = 0; }

    if (input.increase_voxel_gap) {
        res.cur_voxel_gap += 0.005 * delta_time * res.camera.movement_speed; }
    if (input.decrease_voxel_gap) {
        res.cur_voxel_gap -= 0.005 * delta_time * res.camera.movement_speed; }
    if (res.cur_voxel_gap <= 0) {
        res.cur_voxel_gap = 0; }

    if (input.turn_up      ) { res.camera.Turn(ty::CameraDirection::UP, delta_time); }
    if (input.turn_down    ) { res.camera.Turn(ty::CameraDirection::DOWN, delta_time); }
    if (input.turn_left    ) { res.camera.Turn(ty::CameraDirection::LEFT, delta_time); }
    if (input.turn_right   ) { res.camera.Turn(ty::CameraDirection::RIGHT, delta_time); }

    if (input.walk_up      ) { res.camera.Advance(ty::CameraDirection::UP, delta_time); }
    if (input.walk_down    ) { res.camera.Advance(ty::CameraDirection::DOWN, delta_time); }
    if (input.walk_forward ) { res.camera.Advance(ty::CameraDirection::FORWARD, delta_time); }
    if (input.walk_backward) { res.camera.Advance(ty::CameraDirection::BACKWARD, delta_time); }
    if (input.walk_left    ) { res.camera.Advance(ty::CameraDirection::LEFT, delta_time); }
    if (input.walk_right   ) { res.camera.Advance(ty::CameraDirection::RIGHT, delta_time); }

    if (input.rotate_left  ) { res.camera.Rotate(ty::CameraDirection::LEFT, delta_time); }
    if (input.rotate_right ) { res.camera.Rotate(ty::CameraDirection::RIGHT, delta_time); }

	if (input.mouse_scroll_y != 0) {
		if (res.camera_zoom >= 1.0f && res.camera_zoom <= 45.0f) {
			res.camera_zoom -= input.mouse_scroll_y; }
		if (res.camera_zoom <= 1.0f) {
			res.camera_zoom = 1.0f; }
		if (res.camera_zoom >= 45.0f) {
			res.camera_zoom = 45.0f; }
	}

	res.buttons.mouse_click.track(input.mouse_click_left || input.space);
    if (res.buttons.mouse_click.is_pressed()) {
		glfwSetInputMode(res.screen.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (res.last_mouse_x < 0) {
            res.last_mouse_x = input.mouse_motion_x;
            res.last_mouse_y = input.mouse_motion_y;
		}
		else {
			double xoffset = input.mouse_motion_x - res.last_mouse_x;
			double yoffset = res.last_mouse_y - input.mouse_motion_y; // reversed since y-coordinates go from bottom to top

			res.last_mouse_x = input.mouse_motion_x;
			res.last_mouse_y = input.mouse_motion_y;

			res.camera.Drag(xoffset, yoffset);
		}
	}
	if (res.buttons.mouse_click.just_released()) {
		glfwSetInputMode(res.screen.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        res.last_mouse_x = -1;
        res.last_mouse_y = -1;
    }

    auto width = res.image_width;
    auto height = res.image_height;
	auto voxel_scale = glm::vec3{
	    res.cur_voxel_scale_x + 1,
        res.cur_voxel_scale_y + 1,
        (res.cur_voxel_scale_x + res.cur_voxel_scale_x)/2 + 1
    };

    auto voxel_gap = glm::vec2{1.0 + res.cur_voxel_gap, 1 + res.cur_voxel_gap};

    if (res.image_width % 256 == 0 && res.image_height % 224 == 0) {
        voxel_scale.x /= snes_factor_horizontal;
        voxel_gap.x *= snes_factor_horizontal;
    }

    glm::mat4 projection = glm::perspective(glm::radians(float(res.camera_zoom)), (float)res.screen.width / (float)res.screen.height, 1.0f, 100000.0f);
    glm::mat4 view = res.camera.GetViewMatrix();

    res.info_panel.info_mixer += delta_time * 0.1f;
	if (res.showing_waves) {
		res.voxels_pulse += delta_time * 0.1f;
	}
	else {
		res.voxels_pulse = 0;
	}

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TRY_ERROR(res.lighting_shader.use());
	res.lighting_shader.setFloat("pulse", res.voxels_pulse * 7);
    res.lighting_shader.setFloat("ambientStrength", 0.5f);
    res.lighting_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    res.lighting_shader.setVec3("lightPos", glm::vec3{width / 2, height / 2, 400.0f});
    res.lighting_shader.setMat4("projection", projection);
    res.lighting_shader.setMat4("view", view);
	res.lighting_shader.setVec3("voxel_scale", voxel_scale);
    res.lighting_shader.setVec2("voxel_gap", voxel_gap);

    // world transformation
    glBindVertexArray(res.voxel_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, res.image_width * res.image_height);

    if (res.info_panel.showing_info) {
        glBindTexture(GL_TEXTURE_2D, res.info_panel.info_texture);
        TRY_ERROR(res.info_panel.info_shader.use());
        res.info_panel.info_shader.setFloat("mixer", res.info_panel.info_mixer);
        glBindVertexArray(res.info_panel.info_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

	RETURN_OK;
}