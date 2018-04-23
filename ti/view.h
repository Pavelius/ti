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
struct list : control {
	int				origin, current, current_hilite;
	int				maximum_width, origin_width;
	int				lines_per_page, pixels_per_line;
	bool			show_grid_lines;
	list();
	void			correction();
	void			ensurevisible(); // ensure that current selected item was visible on screen if current 'count' is count of items per line
	virtual const char* getname(char* result, const char* result_max, int line, int column) const { return 0; }
	virtual int		getmaximum() const { return 0; }
	void			keydown() override;
	void			keyend() override;
	void			keyenter() override;
	void			keyhome() override;
	void			keypageup() override;
	void			keypagedown() override;
	void			keyup() override;
	void			mouseleftdbl(point position) override;
	void			mousewheel(point position, int step) override;
	void			select(int index);
	virtual void	row(rect rc, int index); // Draw single row - part of list
	void			view(rect rc) override;
};
struct unitlist : list {
	unitlist(army& source) : source(source) {}
	int				getmaximum() const override { return source.count; }
	const char*		getname(char* result, const char* result_max, int line, int column) const override;
private:
	army&			source;
};
}
int					buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips = 0, void(*callback)() = 0);
int					getbuttonheight();
void				report(const char* format);
areas				window(rect rc, bool disabled = false, bool hilight = false, int border = 0);
int					window(int x, int y, int width, const char* string);
int					windowb(int x, int y, int width, const char* string, int id, int param, int border = 0);
}