#include "draw.h"
#include "main.h"

#pragma once

struct gui_info {
	unsigned char	border;
	unsigned char	opacity, opacity_disabled, opacity_hilighted;
	short			button_width, window_width, tips_width, control_border;
	short			padding;
};
extern gui_info gui_data;

namespace draw {
namespace controls {
struct list {
	int				origin, maximum, current;
	int				maximum_width, origin_width;
	int				lines_per_page, pixels_per_line;
	bool			show_grid_lines;
	bool			focused;
	list();
	virtual void	background(rect& rc);
	void			correction();
	void			ensurevisible(); // ensure that current selected item was visible on screen if current 'count' is count of items per line
	void			keydown();
	void			keyend();
	void			keyenter();
	void			keyhome();
	void			keypageup();
	void			keypagedown();
	void			keyup();
	void			mouseleftdbl(point position);
	void			mousewheel(point position, int step);
	void			redraw(rect rc);
	void			select(int index);
	virtual void	row(rect rc, int index); // Draw single row - part of list
	void			updaterowheight();
};
}
int					buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips = 0, void(*callback)() = 0);
int					getbuttonheight();
void				report(const char* format);
areas				window(rect rc, bool disabled = false, bool hilight = false, int border = 0);
int					window(int x, int y, int width, const char* string);
int					windowb(int x, int y, int width, const char* string, int id, int param, int border = 0);
}