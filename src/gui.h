#include "renderer.h"

struct GUI_Context
{
	GUI_Quad quads[MAX_GUI_QUADS];
};

bool mouse_in_quad(Mouse mouse, GUI_Quad_Drawable quad)
{
	if (mouse.norm_x < quad.position.x || mouse.norm_x > (quad.position.x + quad.scale.x)) return false;
	if (mouse.norm_y < quad.position.y || mouse.norm_y > (quad.position.y + quad.scale.y)) return false;

	return true;
}