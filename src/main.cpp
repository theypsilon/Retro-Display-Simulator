#include <glad/glad.h>

#include <SDL.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>

#include <theypsilon/camera.h>
#include <theypsilon/boolean_button.h>
#include <theypsilon/error.h>

#include <cstdio>
#include <cmath>
#include <cstdlib>
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
	ty::boolean_button speed_up, speed_down, f1, f11, lalt, enter, waving, swap_voxels_to_pixels;
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

struct Resources {
    SDL_Window* window;
    unsigned int ticks;
    Uint32 last_time;
    Shader lighting_shader;
	InfoResources info_panel;
	ty::Camera camera;
	float camera_zoom;
    unsigned int voxel_vao;
    unsigned int voxel_vbo;
    unsigned int colors_vbo;
	AnimationColors animation_colors;
    int image_width, image_height;
    unsigned int image_counter, image_tick;
    int last_mouse_x, last_mouse_y;
    float cur_voxel_scale_x;
    float cur_voxel_scale_y;
    float cur_voxel_gap;
    bool full_screen;
	float voxels_pulse;
	bool showing_waves;
    bool showing_voxels;
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
    int mouse_motion_x = -1;
    int mouse_motion_y = -1;
};

const long double ratio_4_3 = 4.0 / 3.0;
const long double ratio_256_224 = 256.0 / 224.0;
const long double snes_factor_horizontal = 1.1666666666666666666666666667; //ratio_4_3 / ratio_256_224;
const long double snes_factor_vertical = 1.0; //1.0 / snes_factor_horizontal;

const float cube_geometry[] = {
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
    -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical, -0.5f,      0.0f,  1.0f,  0.0f,
};

const float square_geometry[] = {
    -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
     0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f * snes_factor_horizontal,  0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
    -0.5f * snes_factor_horizontal, -0.5f * snes_factor_vertical,  0.5f,      0.0f,  0.0f,  1.0f,
};

unsigned int SCR_WIDTH  = 1920;
unsigned int SCR_HEIGHT = 1080;

ty::error program(int argc, char* argv[]);
ty::result<Resources> load_resources(SDL_Window* window, const AnimationPaths& animation_paths);
ty::result<InfoResources> load_info_resources();
ty::result<std::vector<std::vector<glm::vec4>>> load_animation(const std::vector<const char*>& paths, int& image_width, int& image_height);
ty::error load_image_on_gpu(const std::vector<glm::vec4>& colors, const int image_width, const int image_height, const unsigned int colors_vbo);
ty::error update(const Input& input, Resources& res, float delta_time);
void read_input(Input& input, bool& loop);
void windows_high_dpi_hack(SDL_Window* window, unsigned int&width, unsigned int& height);

const AnimationPaths animation_collection[] = {
	AnimationPaths{{"resources/textures/snes2.png"}},
	AnimationPaths{{"resources/textures/snes3.png"}},
	AnimationPaths{{"resources/textures/snes4.png"}},
	AnimationPaths{{"resources/textures/snes.png"}},
	AnimationPaths{{
        "resources/textures/seikendensetsu1.png",
        "resources/textures/seikendensetsu2.png",
        "resources/textures/seikendensetsu3.png",
        "resources/textures/seikendensetsu4.png",
        "resources/textures/seikendensetsu5.png",
        "resources/textures/seikendensetsu6.png",
    }},
	AnimationPaths{{
        "resources/textures/voxel_chronotrigger1.png",
        "resources/textures/voxel_chronotrigger2.png",
        "resources/textures/voxel_chronotrigger3.png",
        "resources/textures/voxel_chronotrigger4.png",
//        "resources/textures/voxel_chronotrigger1.png",
//        "resources/textures/voxel_chronotrigger6.png",
//        "resources/textures/voxel_chronotrigger7.png",
//        "resources/textures/voxel_chronotrigger8.png",
    }},
//	"resources/textures/voxe_supermarioworld1.jpg"
};

auto animation_collection_size = sizeof(animation_collection) / sizeof(animation_collection[0]);

int main(int argc, char* argv[]) {
	auto err = program(argc, argv);
	if (err) {
		std::cerr << "Ooops! Something went wrong!\n[ERROR] " << err.msg << "\nClosing program in 10 seconds.\n";
		SDL_Delay(10000);
		return -1;
	}
	return 0;
}


ty::error program(int argc, char* argv[]) {
	std::srand(std::time(nullptr));	
	auto animation_paths = animation_collection[std::rand() % animation_collection_size];
	if (argc > 1) {
		animation_paths = AnimationPaths{ { argv[1] } };
	}
	std::cout << "Playing image '" << animation_paths.paths[0] << "'.\n";

	TRY_NONNEG(SDL_Init(SDL_INIT_VIDEO));
	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // It doesn't work combined with MSAA, I don't know why.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // This is MSAA on/off
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // This is MSAA number of sampling

#ifndef WIN32
	SDL_DisplayMode display_mode;
	if (SDL_GetDesktopDisplayMode(0, &display_mode) == 0) {
		std::cout << "Resolution: " << display_mode.w << "x" << display_mode.h << std::endl;
		SCR_WIDTH = display_mode.w;
		SCR_HEIGHT = display_mode.h;
	}
#endif

	TRY_NOTNULL(auto, main_window, SDL_CreateWindow(
		"Retro Voxel Display",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCR_WIDTH,
		SCR_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
	));

#ifdef WIN32
	windows_high_dpi_hack(main_window, SCR_WIDTH, SCR_HEIGHT);
#endif

	SDL_WarpMouseInWindow(main_window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

	TRY_NOTNULL(auto, glContext, SDL_GL_CreateContext(main_window));
	TRY_NONNEG(gladLoadGLLoader(SDL_GL_GetProcAddress))

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string) : %s\n", glGetString(GL_VERSION));
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	TRY_NONNEG(SDL_GL_SetSwapInterval(1));

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	TRY_RESULT(auto, res, load_resources(main_window, animation_paths));
	Input input;
	bool loop = true;
	float delta_time = 0.0f;
	auto last_time = std::chrono::system_clock::now();
	while (loop) {
		auto current_time = std::chrono::system_clock::now();
		delta_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time).count() / 1000000.0f;
		last_time = current_time;

		read_input(input, loop);
		TRY_ERROR(update(input, res, delta_time));

		SDL_GL_SwapWindow(main_window);

		TRY_GL_ERROR;
	}

	RETURN_OK;
}

ty::result<Resources> load_resources(SDL_Window* window, const AnimationPaths& animation_paths) {
    int image_width, image_height, image_nr_channels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    unsigned int voxel_vao;
    glGenVertexArrays(1, &voxel_vao);
    glBindVertexArray(voxel_vao);

    unsigned int voxel_vbo;
    glGenBuffers(1, &voxel_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, voxel_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_geometry), cube_geometry, GL_STATIC_DRAW);

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
    float half_width = float(image_width) / 2.0f * voxel_scale * snes_factor_horizontal;
    float half_height = float(image_height) / 2.0f * voxel_scale;

    int offset_multiplier = 1;

    std::vector<glm::vec2> offsets(image_width * image_height);
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            float x = i * voxel_scale * snes_factor_horizontal - half_width;
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
        FileSystem::getPath("resources/shaders/voxel.vs").c_str(),
        FileSystem::getPath("resources/shaders/voxel.fs").c_str()
    ));

	TRY_GL_ERROR;

	ty::Camera camera{};
	camera.movement_speed *= 5;
	camera.SetPosition(glm::vec3{ 0.0f, 0.0f, 270.0f * offset_multiplier });

	TRY_RESULT(auto, info_panel, load_info_resources());

	Resources res{};
    res.window = window;
    res.ticks = 0;
    res.last_time = 0;
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
    res.image_tick = SDL_GetTicks();
    res.last_mouse_x = -1;
    res.last_mouse_y = -1;
    res.cur_voxel_scale_x = 0.0f;
    res.cur_voxel_scale_y = 0.0f;
    res.cur_voxel_gap = 0.0f;
    res.full_screen = false;
	res.voxels_pulse = 0.0f;
    res.showing_waves = false;
    res.showing_voxels = true;
    return res;
}

ty::result<InfoResources> load_info_resources() {
	InfoResources info_res;

	int info_width, info_height, info_nr_channels;
	GLuint info_texture;
	TRY_NOTNULL(auto, data, stbi_load(FileSystem::getPath("resources/textures/info.png").c_str(), &info_width, &info_height, &info_nr_channels, 0));
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

	TRY_GL_ERROR;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_info), indices_info, GL_STATIC_DRAW);

	TRY_GL_ERROR;

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

	TRY_GL_ERROR;
	stbi_image_free(data);

	TRY_RESULT(Shader, info_shader, Shader::load_shader(
		FileSystem::getPath("resources/shaders/info_panel.vs").c_str(),
		FileSystem::getPath("resources/shaders/info_panel.fs").c_str()
	));

	info_res.info_texture = info_texture;
	info_res.info_vao = info_vao;
	info_res.info_shader = std::move(info_shader);
	info_res.info_mixer = 0.0f;
	info_res.showing_info = true;
	return info_res;
}

ty::result<std::vector<std::vector<glm::vec4>>> load_animation(const std::vector<const char*>& paths, int& image_width, int& image_height) {
	int image_nr_channels;
	image_width = -1, image_height = -1;
	std::vector<std::vector<glm::vec4>> colors_by_image;
	for (auto path : paths) {
		int current_width, current_height;
		TRY_NOTNULL(unsigned char *, data, stbi_load(FileSystem::getPath(path).c_str(), &current_width, &current_height, &image_nr_channels, 0));
		if (image_width == -1 && image_height == -1) {
			image_width = current_width;
			image_height = current_height;
		} else if (image_width != current_width || image_height != current_height) {
			RETURN_ERROR("width and height from image '" + path + "' does not match with the first image of the animation");
		}
		std::vector<glm::vec4> colors(image_width * image_height);
		int index = 0;
		for (int j = 0; j < image_height; j++) {
			for (int i = 0; i < image_width; i++) {
				colors[j * image_width + i] = glm::vec4{
					((float)data[index + 0]) / 255.0f,
					((float)data[index + 1]) / 255.0f,
					((float)data[index + 2]) / 255.0f,
					((float)data[index + 3]) / 255.0f
				};
				index += image_nr_channels;
			}
		}
		stbi_image_free(data);

		colors_by_image.emplace_back(std::move(colors));
	}
	return colors_by_image;
}

ty::error load_image_on_gpu(const std::vector<glm::vec4>& colors, const int image_width, const int image_height, const unsigned int colors_vbo) {
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * image_width * image_height, colors.data(), GL_STATIC_DRAW);

	TRY_GL_ERROR;

	RETURN_OK;
}

ty::error update(const Input& input, Resources& res, float delta_time) {
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

    if (res.animation_colors.colors_by_image.size() > 1 && now > res.image_tick + res.animation_colors.milliseconds) {
        res.image_tick = now;

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

		TRY_GL_ERROR;

        std::cout << "Activating " << (res.showing_voxels ? "pixels" : "voxels") << ".\n";
        res.showing_voxels = !res.showing_voxels;
    }

	res.buttons.f1.track(input.f1);
	if (res.buttons.f1.just_pressed()) {
		res.info_panel.showing_info = !res.info_panel.showing_info;
	}

	res.buttons.speed_up.track(input.speed_up);
    if (res.buttons.speed_up.just_pressed())
        res.camera.movement_speed *= 1.5f;

	res.buttons.speed_down.track(input.speed_down);
    if (res.buttons.speed_down.just_pressed())
        res.camera.movement_speed /= 1.5f;

    if (res.camera.movement_speed > 10000)
        res.camera.movement_speed = 10000;
    if (res.camera.movement_speed < 0.1)
        res.camera.movement_speed = 0.1f;

    if (input.increase_voxel_scale_x)
        res.cur_voxel_scale_x += 0.005f * delta_time * res.camera.movement_speed;
    if (input.decrease_voxel_scale_x)
        res.cur_voxel_scale_x -= 0.005f * delta_time * res.camera.movement_speed;
    if (res.cur_voxel_scale_x <= 0)
        res.cur_voxel_scale_x = 0;

	if (input.increase_voxel_scale_y)
		res.cur_voxel_scale_y += 0.005f * delta_time * res.camera.movement_speed;
	if (input.decrease_voxel_scale_y)
		res.cur_voxel_scale_y -= 0.005f * delta_time * res.camera.movement_speed;
	if (res.cur_voxel_scale_y <= 0)
		res.cur_voxel_scale_y = 0;

    if (input.increase_voxel_gap)
        res.cur_voxel_gap += 0.005f * delta_time * res.camera.movement_speed;
    if (input.decrease_voxel_gap)
        res.cur_voxel_gap -= 0.005f * delta_time * res.camera.movement_speed;
    if (res.cur_voxel_gap <= 0)
        res.cur_voxel_gap = 0;

    if (input.turn_up      ) res.camera.Turn(ty::CameraDirection::UP, delta_time);
    if (input.turn_down    ) res.camera.Turn(ty::CameraDirection::DOWN, delta_time);
    if (input.turn_left    ) res.camera.Turn(ty::CameraDirection::LEFT, delta_time);
    if (input.turn_right   ) res.camera.Turn(ty::CameraDirection::RIGHT, delta_time);

    if (input.walk_up      ) res.camera.Advance(ty::CameraDirection::UP, delta_time);
    if (input.walk_down    ) res.camera.Advance(ty::CameraDirection::DOWN, delta_time);
    if (input.walk_forward ) res.camera.Advance(ty::CameraDirection::FORWARD, delta_time);
    if (input.walk_backward) res.camera.Advance(ty::CameraDirection::BACKWARD, delta_time);
    if (input.walk_left    ) res.camera.Advance(ty::CameraDirection::LEFT, delta_time);
    if (input.walk_right   ) res.camera.Advance(ty::CameraDirection::RIGHT, delta_time);

    if (input.rotate_left  ) res.camera.Rotate(ty::CameraDirection::LEFT, delta_time);
    if (input.rotate_right ) res.camera.Rotate(ty::CameraDirection::RIGHT, delta_time);

    if (input.mouse_click_left) {
        if (res.last_mouse_x < 0) {
            res.last_mouse_x = input.mouse_motion_x;
            res.last_mouse_y = input.mouse_motion_y;
		}
		else {
			float xoffset = input.mouse_motion_x - res.last_mouse_x;
			float yoffset = res.last_mouse_y - input.mouse_motion_y; // reversed since y-coordinates go from bottom to top

			res.last_mouse_x = input.mouse_motion_x;
			res.last_mouse_y = input.mouse_motion_y;

			res.camera.Drag(xoffset, yoffset);
		}
    } else {
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

    auto voxel_gap = glm::vec2{1 + res.cur_voxel_gap, 1 + res.cur_voxel_gap};

    glm::mat4 projection = glm::perspective(glm::radians(res.camera_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100000.0f);
    glm::mat4 view = res.camera.GetViewMatrix();

    res.info_panel.info_mixer += delta_time * 0.1f;
	if (res.showing_waves) {
		res.voxels_pulse += delta_time * 0.1f;
	}
	else {
		res.voxels_pulse = 0;
	}

	res.ticks++;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

void read_input(Input& input, bool& loop) {
	SDL_PumpEvents();

	const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
	loop = kbstate[SDL_SCANCODE_ESCAPE] == false && SDL_QuitRequested() == false;

	input.mouse_click_left = SDL_GetMouseState(&input.mouse_motion_x, &input.mouse_motion_y) & SDL_BUTTON(SDL_BUTTON_LEFT) || kbstate[SDL_SCANCODE_SPACE];

    input.walk_left              = kbstate[SDL_SCANCODE_A       ];// || kbstate[SDL_SCANCODE_LEFT ];
    input.walk_right             = kbstate[SDL_SCANCODE_D       ];// || kbstate[SDL_SCANCODE_RIGHT];
    input.walk_forward           = kbstate[SDL_SCANCODE_W       ];// || kbstate[SDL_SCANCODE_UP   ];
    input.walk_backward          = kbstate[SDL_SCANCODE_S       ];// || kbstate[SDL_SCANCODE_DOWN ];
    input.walk_up                = kbstate[SDL_SCANCODE_Q       ];
    input.walk_down              = kbstate[SDL_SCANCODE_E       ];
    input.turn_left              = kbstate[SDL_SCANCODE_LEFT    ];
    input.turn_right             = kbstate[SDL_SCANCODE_RIGHT   ];
    input.turn_up                = kbstate[SDL_SCANCODE_UP      ];
    input.turn_down              = kbstate[SDL_SCANCODE_DOWN    ];
    input.rotate_right           = kbstate[SDL_SCANCODE_KP_PLUS ];
    input.rotate_left            = kbstate[SDL_SCANCODE_KP_MINUS];
    input.swap_voxels_to_pixels = kbstate[SDL_SCANCODE_O       ];
    input.increase_voxel_scale_y = kbstate[SDL_SCANCODE_J       ];
    input.decrease_voxel_scale_y = kbstate[SDL_SCANCODE_K       ];
	input.increase_voxel_scale_x = kbstate[SDL_SCANCODE_U       ];
	input.decrease_voxel_scale_x = kbstate[SDL_SCANCODE_I       ];
    input.increase_voxel_gap     = kbstate[SDL_SCANCODE_N       ];
    input.decrease_voxel_gap     = kbstate[SDL_SCANCODE_M       ];
	input.speed_up               = kbstate[SDL_SCANCODE_F       ];
	input.speed_down             = kbstate[SDL_SCANCODE_R       ];
	input.f1                     = kbstate[SDL_SCANCODE_F1      ];
	input.f11                    = kbstate[SDL_SCANCODE_F11     ];
	input.alt                    = kbstate[SDL_SCANCODE_LALT    ];
	input.enter                  = kbstate[SDL_SCANCODE_RETURN  ];
	input.change_view            = kbstate[SDL_SCANCODE_C       ];
	input.change_waving          = kbstate[SDL_SCANCODE_P       ];
}

#ifdef WIN32
#include <cassert>
#include "SDL_syswm.h"
void windows_high_dpi_hack(SDL_Window* window, unsigned int&width, unsigned int& height) {

	SDL_SysWMinfo sys_wm_info;
	SDL_VERSION(&sys_wm_info.version);
	SDL_GetWindowWMInfo(window, &sys_wm_info);

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

	bool result = false;
	if (SetProcessDpiAwareness) {
		/* Try Windows 8.1+ version */
		result = S_OK == SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		SDL_Log("called SetProcessDpiAwareness: %d", result ? 1 : 0);
	}
	else if (SetProcessDPIAware) {
		/* Try Vista - Windows 8 version.
		This has a constant scale factor for all monitors.
		*/
		result = TRUE == SetProcessDPIAware();
		SDL_Log("called SetProcessDPIAware: %d", result ? 1 : 0);
	}

	if (!result) {
		return;
	}

	auto monitor_from_window = (HMONITOR(WINAPI *)(HWND, DWORD)) SDL_LoadFunction(userDLL, "MonitorFromWindow");
	auto get_monitor_info = (BOOL(WINAPI *)(HMONITOR, LPMONITORINFOEX)) SDL_LoadFunction(userDLL, "GetMonitorInfoA");
	if (!monitor_from_window || !get_monitor_info) {
		return;
	}

	auto hMonitor = monitor_from_window(sys_wm_info.info.win.window, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	get_monitor_info(hMonitor, &miex);
	auto real_width = (miex.rcMonitor.right - miex.rcMonitor.left);
	auto real_height = (miex.rcMonitor.bottom - miex.rcMonitor.top);
	if (real_width != width || real_height != height) {
		width = real_width;
		height = real_height;
		SDL_SetWindowSize(window, width, height);
	}
}
#endif