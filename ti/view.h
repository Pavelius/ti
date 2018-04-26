#include "draw.h"
#include "main.h"

#pragma once

enum property_s : unsigned char { NoProperty,
	Name, Count, Resources,
};

struct gui_info {
	unsigned char	border;
	unsigned char	opacity, opacity_disabled, opacity_hilighted;
	short			button_width, window_width, tips_width, control_border;
	short			padding;
};
extern gui_info gui_data;

namespace draw {
namespace dialogs {
bool				production(int production_limit);
}
void				board();
int					buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips = 0, void(*callback)() = 0);
int					getbuttonheight();
point				h2p(point hex);
void				hexagon(point pt);
point				p2h(point pt);
void				report(const char* format);
areas				window(rect rc, bool disabled = false, bool hilight = false, int border = 0);
int					window(int x, int y, int width, const char* string);
int					windowb(int x, int y, int width, const char* string, int id, int param, int border = 0);
}