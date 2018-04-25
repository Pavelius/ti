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
namespace controls {
struct unitlist : list {
	struct column {
		const char*	title;
		int			width;
		unsigned	flags;
		int			(unit::*gnum)() const;
		const char*	(unit::*gtxt)() const;
		constexpr column() : title(0), gnum(0), gtxt(0), width(0), flags(0) {}
		constexpr column(const char* title, int (unit::*get)() const, unsigned flags = AlignRight) : title(title), gnum(get), gtxt(0), width(32), flags(flags) {}
		constexpr column(const char* title, const char* (unit::*get)() const) : title(title), gnum(0), gtxt(get), width(100), flags(AlignLeft) {}
	};
	const column*	columns;
	unitlist(army& source, const column* columns) : source(source), columns(columns) {}
	int				getmaximum() const override { return source.count; }
	const char*		getname(char* result, const char* result_max, int line, int column) const override;
	virtual int		getnumber(int line, int column) const { return 0; }
	virtual void	row(rect rc, int index) const; // Draw single row - part of list
private:
	army&			source;
};
}
int					buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips = 0, void(*callback)() = 0);
int					getbuttonheight();
point				h2p(point hex);
void				report(const char* format);
areas				window(rect rc, bool disabled = false, bool hilight = false, int border = 0);
int					window(int x, int y, int width, const char* string);
int					windowb(int x, int y, int width, const char* string, int id, int param, int border = 0);
}