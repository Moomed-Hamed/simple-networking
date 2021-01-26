#include "networking.h"
#include "gui.h"

const char* ip   = "192.168.1.1"; // this ip is wrong, read the correct one from a file or something
const char* port = "42069";

int main()
{

	Window window = {};
	window_init(&window, 1920, 1080, "GUI");

	Keyboard keys = {};
	keyboard_init(&keys);

	Mouse mouse = {};

	GUI_Renderer* gui = (GUI_Renderer*)calloc(1, sizeof(GUI_Renderer));
	init(gui);

	//gui->num_quads++;
	//gui->quads[0].scale = vec2(1,1);
	//gui->quads[0].color = vec3(1, 1, 1);
	//
	//gui->num_quads++;
	//gui->quads[1].scale = vec2(.1, .1);
	//gui->quads[1].color = vec3(0, 1, 1);
	//
	//gui->quads[0].position = vec2(mouse.norm_x, mouse.norm_y);
	//
	//static float mix = 0;
	//mix += frame_time;
	//if (mix > 1) mix = 0;
	//float color = interpolate(0, 1, mix);
	//
	//gui->quads[0].color = vec3(1, color, color);
	//gui->quads[1].scale = vec2(1 - color, 1 - color);

	int quad_resolution = 32;
	float quad_spacing_x = 1.f / 16; // 1920/128 = 15
	float quad_spacing_y = 1.f / 9; // 1080/128 = 8.4375

	for (int x = 0, i = 0; x < 32; x++) {
	for (int y = 0; y < 18; y++)
	{
		gui->quads[i].position = vec2(-1 + (x * quad_spacing_x), -1 + (y * quad_spacing_y));
		gui->quads[i].scale = vec2(quad_spacing_x, quad_spacing_y);
		gui->quads[i].color = {};// vec3(1 - (x / 16.f), y / 16.f, 1);
		gui->num_quads++;
		i++;
	} }

	int mouse_index = gui->num_quads;
	gui->quads[0].scale = vec2(.03, .05);
	
	gui->num_quads++;

	// frame timer
	float frame_time = 1.f / 120; // 120fps target
	int64 target_frame_milliseconds = frame_time * 1000.f;
	Timestamp frame_start, frame_end;
	frame_start = get_timestamp();

	while (1)
	{
		game_window_update();
		mouse_update(&mouse, window);
		keyboard_update(&keys, window);

		if (keys.ESC.is_pressed) break;

		gui->quads[0].position = vec2(mouse.norm_x, mouse.norm_y);

		if (mouse.left_button.is_pressed)
		{
			for (int i = 0; i < MAX_GUI_QUADS; i++)
			{
				if (mouse_in_quad(mouse, gui->quads[i]))
				{
					gui->quads[i].color = vec3(0, 1, 1);
				}
			}
		}

		if (mouse.right_button.is_pressed)
		{
			for (int i = 0; i < MAX_GUI_QUADS; i++)
			{
				if (mouse_in_quad(mouse, gui->quads[i]))
				{
					gui->quads[i].color = vec3(0, 0, 0);
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		gui->quads[0].color = vec3(1, 1, 0);

		update(gui);
		bind(gui->shader);
		draw(gui);

		glfwSwapBuffers(window.instance);

		//Frame Time
		frame_end = get_timestamp();
		int64 milliseconds_elapsed = calculate_milliseconds_elapsed(frame_start, frame_end);

		//print("frame time: %02d ms | fps: %06f\n", milliseconds_elapsed, 1000.f / milliseconds_elapsed);
		if (target_frame_milliseconds > milliseconds_elapsed) // frame finished early
		{
			uint sleep_milliseconds = target_frame_milliseconds - milliseconds_elapsed;
			os_sleep(sleep_milliseconds);
		}

		frame_start = frame_end;
	}

	return 0;
}