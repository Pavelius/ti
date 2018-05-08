#include "draw.h"
#include "main.h"

#pragma once

enum property_s : unsigned char { NoProperty,
	Name, Count, Resources,
};

struct gui_info {
	unsigned char	border;
	unsigned char	opacity, opacity_disabled, opacity_hilighted;
	short			button_width, window_width, window_height, tips_width, control_border;
	short			padding;
};
extern gui_info gui_data;

namespace draw {
void				board();
bool				boardkeys(int id);
int					buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips = 0, void(*callback)() = 0);
unsigned char		choose(querry& source, const char* title, const char* (*source_getname)(unsigned char index));
player_s			chooseplayer();
int					getbuttonheight();
void				icon(int x, int y, unit_s type, int count);
point				h2p(point hex);
void				hexagon(point pt);
point				p2h(point pt);
void				report(const char* format);
void				statistic();
bool				production(player_s player, int production_limit);
areas				window(rect rc, bool disabled = false, bool hilight = false, int border = 0);
rect				window(const char* header);
rect				window(const char* header, command_s id);
int					window(int x, int y, int width, const char* string);
int					windowb(int x, int y, int width, const char* string, int id, int param, int border = 0);
}