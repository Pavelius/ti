#include "view.h"

using namespace draw;
using namespace draw::controls;

static point	camera;
static point	camera_drag;
static rect		last_board;
static point	last_mouse;
static point	tooltips_point;
static short	tooltips_width;
static char		tooltips_text[4096];
extern rect		sys_static_area;
gui_info		gui_data;

static struct view_plugin : draw::renderplugin {

	static void debug_mouse() {
		char temp[128];
		draw::state state;
		draw::fore = colors::text;
		draw::font = metrics::font;
		szprints(temp, endofs(temp), "mouse %1i, %2i, map mouse %3i, %4i", hot::mouse.x, hot::mouse.y, last_mouse.x, last_mouse.y);
		draw::text(2, draw::getheight() - 20, temp);
	}

	void initialize() override {
		// Color setting
		colors::active = color::create(172, 128, 0);
		colors::border = color::create(73, 73, 80);
		colors::button = color::create(0, 122, 204);
		colors::form = color::create(32, 32, 32);
		colors::window = color::create(64, 64, 64);
		colors::text = color::create(255, 255, 255);
		colors::edit = color::create(38, 79, 120);
		colors::h1 = colors::text.mix(colors::edit, 64);
		colors::h2 = colors::text.mix(colors::edit, 96);
		colors::h3 = colors::text.mix(colors::edit, 128);
		colors::special = color::create(255, 244, 32);
		colors::border = colors::window.mix(colors::text, 128);
		colors::tips::text = color::create(255, 255, 255);
		colors::tips::back = color::create(100, 100, 120);
		colors::tabs::back = color::create(255, 204, 0);
		colors::tabs::text = colors::black;
		// Setting of gui data
		gui_data.border = 8;
		gui_data.control_border = 4;
		gui_data.padding = 2;
		gui_data.button_width = 120;
		gui_data.opacity = 128;
		gui_data.opacity_disabled = 192;
		gui_data.window_width = 320;
	}

	void after() override {
		if(!tooltips_text[0])
			return;
		draw::state push;
		draw::font = metrics::font;
		if(draw::font) {
			rect rc;
			rc.x1 = tooltips_point.x + tooltips_width + gui_data.border * 2 + gui_data.padding;
			rc.y1 = tooltips_point.y;
			rc.x2 = rc.x1 + gui_data.tips_width;
			rc.y2 = rc.y1;
			draw::textf(rc, tooltips_text);
			if(rc.x2 > getwidth() - gui_data.border - gui_data.padding) {
				auto w = rc.width();
				rc.x1 = tooltips_point.x - gui_data.border * 2 - gui_data.padding - w;
				rc.x2 = rc.x1 + w;
			}
			// Correct border
			int height = draw::getheight();
			int width = draw::getwidth();
			if(rc.y2 >= height)
				rc.move(0, height - 2 - rc.y2);
			if(rc.x2 >= width)
				rc.move(width - 2 - rc.x2, 0);
			draw::window(rc);
			draw::fore = colors::tips::text;
			draw::textf(rc.x1, rc.y1, rc.width(), tooltips_text);
		}
		tooltips_text[0] = 0;
	}

} view_plugin_instance;

static bool control_board(int id) {
	const int step = 32;
	switch(id) {
	case KeyLeft: camera.x -= step; break;
	case KeyRight: camera.x += step; break;
	case KeyUp: camera.y -= step; break;
	case KeyDown: camera.y += step; break;
	case MouseLeft:
		if(hot::pressed) {
			if(last_board == hot::hilite) {
				draw::drag::begin(100);
				camera_drag = camera;
			}
		}
		break;
	default:
		if(draw::drag::active(100)) {
			hot::cursor = CursorAll;
			if(hot::mouse.x >= 0 && hot::mouse.y >= 0)
				camera = camera_drag + (draw::drag::mouse - hot::mouse);
			return true;
		}
		return false;
	}
	return true;
}

areas draw::window(rect rc, bool disabled, bool hilight, int border) {
	if(border == 0)
		border = gui_data.border;
	rc.offset(-border, -border);
	color c = colors::form;
	auto rs = draw::area(rc);
	auto op = gui_data.opacity;
	if(disabled)
		op = op / 2;
	else if(hilight && !disabled && (rs == AreaHilited || rs == AreaHilitedPressed))
		op = gui_data.opacity_hilighted;
	draw::rectf(rc, c, op);
	draw::rectb(rc, c);
	return rs;
}

int draw::window(int x, int y, int width, const char* string) {
	rect rc = {x, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	int height = draw::textf(rc, string);
	window(rc, false);
	link[0] = 0; draw::textf(x, y, rc.width(), string);
	if(link[0])
		tooltips(x, y, rc.width(), link);
	return height + gui_data.border * 2 + gui_data.padding;
}

int draw::windowb(int x, int y, int width, const char* string, int id, int param, int border) {
	draw::state push;
	draw::font = metrics::font;
	rect rc = {x, y, x + width, y + draw::texth()};
	auto ra = window(rc, false, id != 0, border);
	draw::text(rc, string, AlignCenterCenter);
	if(id && ra == AreaHilitedPressed && hot::key == MouseLeft)
		draw::execute(id, param);
	return rc.height() + gui_data.border * 2;
}

void draw::tooltips(int x1, int y1, int width, const char* format, ...) {
	tooltips_point.x = x1;
	tooltips_point.y = y1;
	tooltips_width = width;
	szprintvs(tooltips_text, endofs(tooltips_text), format, xva_start(format));
}

static void background() {
	draw::rectf({0, 0, draw::getwidth(), draw::getheight()}, colors::window);
}

void draw::report(const char* format) {
	while(ismodal()) {
		background();
		draw::window(gui_data.border * 2, gui_data.border * 2, gui_data.window_width, format);
		auto id = input();
		if(control_board(id))
			continue;
	}
}

int	draw::getbuttonheight() {
	return gui_data.control_border * 2 + draw::texth();
}

int	draw::button(int x, int y, int width, int id, unsigned flags, const char* label, const char* tips, void(*callback)()) {
	rect rc = {x, y, x + width, y + getbuttonheight()};
	if(buttonh(rc, ischecked(flags), isfocused(flags), isdisabled(flags), true,
		label, 0, false, tips)
		|| (isfocused(flags) && hot::key == KeyEnter)) {
		if(callback)
			execute(callback);
		else
			execute(id);
	}
	return getbuttonheight();
}

int	draw::buttonr(int x, int y, int id, unsigned flags, const char* label, const char* tips, void(*callback)()) {
	auto width = draw::textw(label) + gui_data.control_border * 2;
	if(width < 64)
		width = 64;
	button(x - width, y, width, id, flags, label, tips, callback);
	return width;
}

const char* unitlist::getname(char* result, const char* result_max, int line, int column) const {
	if(columns[column].gnum)
		szprints(result, result_max, "%1i", (source.data[line]->*columns[column].gnum)());
	else if(columns[column].gtxt)
		return (source.data[line]->*columns[column].gtxt)();
	return result;
}

void unitlist::row(rect rc, int index) const {
	char temp[260];
	if(index == current)
		hilight(rc);
	else if(index == current_hilite)
		rectf({rc.x1, rc.y1, rc.x2, rc.y2 - 1}, colors::edit.mix(colors::window, 96));
	rc.offset(4, 4);
	auto pu = source.data[index];
	for(auto i = 0; columns[i].title; i++) {
		rect rt = {rc.x1, rc.y1, rc.x1 + columns[i].width - 4, rc.y2};
		temp[0] = 0;
		auto p = getname(temp, temp + sizeof(temp) / sizeof(temp[0]) - 1, index, i);
		if(p)
			draw::text(rt, p, columns[i].flags);
		rc.x1 += columns[i].width;
	}
}

static void choose_accept() {
	draw::breakmodal(AcceptButton);
}

static void choose_no() {
	draw::breakmodal(0);
}

TEXTPLUGIN(accept) {
	if(hot::key == KeyEnter)
		execute(choose_accept);
	x += width;
	x -= buttonr(x, y, AcceptButton, 0, getstr(AcceptButton), tips, choose_accept) + gui_data.padding;
	return getbuttonheight();
}

TEXTPLUGIN(yesno) {
	if(hot::key == Alpha + 'Y')
		execute(choose_accept);
	else if(hot::key == Alpha + 'N')
		execute(choose_no);
	x += width;
	x -= buttonr(x, y, YesButton, 0, getstr(YesButton), tips, choose_accept) + gui_data.padding;
	x -= buttonr(x, y, NoButton, 0, getstr(NoButton), tips, choose_no) + gui_data.padding;
	return getbuttonheight();
}