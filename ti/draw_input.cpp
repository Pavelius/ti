#include "main.h"
#include "draw_control.h"

using namespace draw;
using namespace draw::controls;
static variant hilited;
static sprite* planets = (sprite*)loadb("art/sprites/planets.pma");
static sprite* font_small = (sprite*)loadb("art/fonts/small.pma");
static color player_colors[6][2];
const auto unit_size = 12;

int isqrt(int num);

enum ui_command_s {
	NoUICommand, ChooseLeft, ChooseRight, ChooseList,
};
struct cmdid {
	callback		proc;
	int					param;
	void clear() { memset(this, 0, sizeof(*this)); }
};
struct focusable_element {
	int					id;
	rect				rc;
	operator bool() const { return id != 0; }
};
struct cube {
	double x, y, z;
};
static focusable_element	elements[96];
static focusable_element*	render_control;
static int				current_focus;
static cmdid			current_execute;
static bool				keep_hot;
static hotinfo			keep_hot_value;
static bool				break_modal;
static int				break_result;
static point			camera;
static point			camera_drag;
static rect				last_board;
static point			tooltips_point;
static short			tooltips_width;
static char				tooltips_text[4096];
static surface			map_image;
static sprite*			sprite_shields;
static rect				hilite_rect;
const int				map_normal = 1000;
static int				map_scale = map_normal;
static control*			current_hilite;
static control*			current_focus_control;
static control*			current_execute_control;
extern rect				sys_static_area;
int						distance(point p1, point p2);
const short				size = 192;
//const short size = 192;
const double			sqrt_3 = 1.732050807568877;
const double			cos_30 = 0.86602540378;

struct piclib : arem<pair<const char*, surface>> {
	typedef pair<const char*, surface> elementi;
	elementi* find(const char* key) {
		for(auto& e : *this) {
			if(strcmp(e.key, key) == 0)
				return &e;
		}
		return 0;
	}
	surface& get(const char* key, const char* folder, int width, int height) {
		auto p = find(key);
		if(!p) {
			char temp[260]; zprint(temp, "art/%1/%2.bmp", folder, key);
			p = add(); memset(p, 0, sizeof(*p));
			p->key = key;
			if(width && height) {
				p->value.resize(width, height, 32, true);
				surface e(temp, 0);
				if(e)
					blit(p->value, 0, 0, p->value.width, p->value.height, 0,
						e, 0, 0, e.width, e.height);
			} else
				p->value.read(temp);
		}
		return p->value;
	}
};

struct gui_info {
	unsigned char		border;
	unsigned char		opacity, opacity_disabled, opacity_hilighted;
	short				button_width, window_width, window_height, hero_width;
	short				tips_width, control_border, right_width;
	short				padding;
	void initialize() {
		memset(this, 0, sizeof(*this));
		opacity = 220;
		opacity_disabled = 50;
		border = 8;
		padding = 4;
		window_width = 400;
		hero_width = 64;
		right_width = 220;
		tips_width = 200;
		button_width = 64;
		opacity_hilighted = 200;
	}
} gui;

static void set_focus_callback() {
	auto id = getnext(draw::getfocus(), hot.param);
	if(id)
		setfocus(id, true);
}

static void setfocus_callback() {
	current_focus = hot.param;
}

static focusable_element* getby(int id) {
	if(!id)
		return 0;
	for(auto& e : elements) {
		if(!e)
			return 0;
		if(e.id == id)
			return &e;
	}
	return 0;
}

static focusable_element* getfirst() {
	for(auto& e : elements) {
		if(!e)
			return 0;
		return &e;
	}
	return 0;
}

static focusable_element* getlast() {
	auto p = elements;
	for(auto& e : elements) {
		if(!e)
			break;
		p = &e;
	}
	return p;
}

void draw::addelement(int id, const rect& rc) {
	if(!render_control
		|| render_control >= elements + sizeof(elements) / sizeof(elements[0]) - 1)
		render_control = elements;
	render_control[0].id = id;
	render_control[0].rc = rc;
	render_control[1].id = 0;
	render_control++;
}

int draw::getnext(int id, int key) {
	if(!key)
		return id;
	auto pc = getby(id);
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	if(key == KeyLeft || key == KeyUp || key == (KeyTab | Shift))
		inc = -1;
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = elements;
		else if(pc < elements)
			pc = pl;
		if(pe == pc)
			return pe->id;
		switch(key) {
		case KeyRight:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 < pc->rc.x1)
				return pc->id;
			break;
		case KeyLeft:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 > pc->rc.x1)
				return pc->id;
			break;
		case KeyDown:
			if(pc->rc.y1 >= pe->rc.y2)
				return pc->id;
			break;
		case KeyUp:
			if(pc->rc.y2 <= pe->rc.y1)
				return pc->id;
			break;
		default:
			return pc->id;
		}
	}
}

void draw::setfocus(int id, bool instant) {
	if(instant)
		current_focus = id;
	else if(current_focus != id)
		execute(setfocus_callback, id);
}

int draw::getfocus() {
	return current_focus;
}

void draw::execute(void(*proc)(), int param) {
	current_execute.proc = proc;
	current_execute.param = param;
}

void draw::execute(const hotinfo& value) {
	keep_hot = true;
	keep_hot_value = value;
	hot.key = InputUpdate;
}

void draw::breakmodal(int result) {
	break_modal = true;
	break_result = result;
}

void draw::buttoncancel() {
	breakmodal(0);
}

void draw::buttonok() {
	breakmodal(1);
}

int draw::getresult() {
	return break_result;
}

bool control::ishilited() const {
	return current_hilite == this;
}

bool control::isfocused() const {
	return current_focus_control == this;
}

void control::view(const rect& rc) {
	if(isfocusable()) {
		addelement((int)this, rc);
		if(!getfocus())
			setfocus((int)this, true);
	}
	if(areb(rc))
		current_hilite = this;
	if((control*)getfocus() == this)
		current_focus_control = this;
	if(show_border)
		rectb(rc, colors::border);
}

static bool control_focus() {
	if(current_hilite) {
		switch(hot.key & CommandMask) {
		case MouseLeft:
		case MouseRight:
		case MouseLeftDBL:
			current_hilite->mouseinput(hot.key, hot.mouse);
			return true;
		case MouseWheelDown:
			current_hilite->mousewheel(hot.key, hot.mouse, 1);
			return true;
		case MouseWheelUp:
			current_hilite->mousewheel(hot.key, hot.mouse, -1);
			return true;
		}
	}
	if(current_focus_control) {
		if(current_focus_control->keyinput(hot.key))
			return true;
	}
	int id;
	switch(hot.key) {
	case 0:
		exit(0);
		return true;
	case KeyTab:
	case KeyTab | Shift:
	case KeyTab | Ctrl:
	case KeyTab | Ctrl | Shift:
		id = getnext(draw::getfocus(), hot.key);
		if(id)
			setfocus(id, true);
		return true;
	}
	return false;
}

static void before_render() {
	hot.cursor = CursorArrow;
	hot.hilite.clear();
	render_control = elements;
	current_execute.clear();
	current_hilite = 0;
	current_focus_control = 0;
	if(hot.mouse.x < 0 || hot.mouse.y < 0)
		sys_static_area.clear();
	else
		sys_static_area = {0, 0, draw::getwidth(), draw::getheight()};
}

bool draw::ismodal() {
	before_render();
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

static areas hilite(rect rc) {
	auto border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	auto rs = draw::area(rc);
	if(rs == AreaHilited) {
		auto op = gui.opacity;
		draw::rectf(rc, c, op);
		draw::rectb(rc, c);
	}
	return rs;
}

static areas window(rect rc, bool disabled = false, bool hilight = true, int border = 0) {
	if(border == 0)
		border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	auto rs = draw::area(rc);
	auto op = gui.opacity;
	if(disabled)
		op = op / 2;
	else if(hilight && (rs == AreaHilited || rs == AreaHilitedPressed)) {
		draw::rectf(rc, colors::button);
		draw::rectb(rc, b);
		return rs;
	}
	draw::rectf(rc, c, op);
	draw::rectb(rc, b);
	return rs;
}

static int render_text(int x, int y, int width, const char* string) {
	draw::link[0] = 0;
	auto result = textf(x, y, width, string);
	if(draw::link[0])
		tooltips(x, y, width, draw::link);
	return result;
}

static int windowf(int x, int y, int width, const char* string) {
	rect rc = {x, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	rc.x2 = rc.x1 + width;
	window(rc, false);
	render_text(x, y, rc.width(), string);
	return height + gui.border * 2 + gui.padding;
}

static int window(int x, int y, int width, const char* string, int right_width = 0, areas* pa = 0) {
	auto right_side = (right_width != 0);
	rect rc = {x, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	if(right_side) {
		auto w1 = rc.width();
		x = x + right_width - w1;
		rc.x1 = x;
		rc.x2 = rc.x1 + w1;
	}
	auto a = window(rc, false, false);
	if(pa)
		*pa = a;
	render_text(x, y, rc.width(), string);
	return height + gui.border * 2;
}

static int render_picture(int x, int y, const char* id, areas* pa = 0) {
	static piclib avatars;
	auto& e = avatars.get(id, "portraits", gui.hero_width, gui.hero_width);
	blit(*draw::canvas, x, y, gui.hero_width, gui.hero_width, 0, e, 0, 0);
	rect rc = {x, y, x + gui.hero_width, y + gui.hero_width};
	rectb(rc, colors::border);
	if(pa)
		*pa = area(rc);
	return gui.hero_width;
	return 0;
}

static int window(int x, int y, int width_picture, int width_text, const char* picture, const char* string, areas* pa = 0) {
	x -= width_picture;
	auto width = width_picture + width_text;
	rect rc = {x, y, x + width, y};
	rect rc1 = {x + width_picture + gui.padding, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc1, string);
	if(height < width_picture)
		height = width_picture;
	auto a = window({x, y, x + width, y + height}, false, false);
	if(pa)
		*pa = a;
	render_picture(x, y, picture);
	render_text(x + width_picture + gui.padding, y, width_text, string);
	return height + gui.border * 2;
}

static int windowb(int x, int y, int width, const char* string, bool& result, bool disabled, int border = 0, unsigned key = 0, const char* tips = 0) {
	draw::state push;
	draw::font = metrics::font;
	rect rc = {x, y, x + width, y + draw::texth()};
	auto ra = window(rc, disabled, true, border);
	draw::text(rc, string, AlignCenterCenter);
	if((ra == AreaHilited || ra == AreaHilitedPressed) && tips)
		tooltips(x, y, rc.width(), tips);
	result = false;
	if(!disabled) {
		if(ra == AreaHilitedPressed && hot.key == MouseLeft)
			result = true;
		if(key && key == hot.key)
			result = true;
	}
	return rc.height() + gui.border * 2;
}

static int windowb(int x, int y, int width, const char* string, callback proc, bool disabled, int border = 0, unsigned key = 0, const char* tips = 0) {
	auto result = false;
	auto h = windowb(x, y, width, string, result, disabled, border, key, tips);
	if(result)
		execute(proc);
	return result;
}

static point getscreen(const rect& rc, point pt) {
	auto x = pt.x - camera.x + rc.x1 + rc.width() / 2;
	auto y = pt.y - camera.y + rc.y1 + rc.height() / 2;
	return {(short)x, (short)y};
}

static point getmappos(const rect& rc, point pt) {
	auto x = pt.x + camera.x - rc.x1 - rc.width() / 2;
	auto y = pt.y + camera.y - rc.y1 - rc.height() / 2;
	return {(short)x, (short)y};
}

static void breakparam() {
	breakmodal(hot.param);
}

static void keyparam() {
	hot.key = hot.param;
	hot.param = 0;
}

static bool control_board() {
	const int step = 32;
	switch(hot.key) {
	case MouseWheelUp: map_scale += 50; break;
	case MouseWheelDown: map_scale -= 50; break;
	case KeyLeft: camera.x -= step; break;
	case KeyRight: camera.x += step; break;
	case KeyUp: camera.y -= step; break;
	case KeyDown: camera.y += step; break;
	case MouseLeft:
		if(hot.pressed && last_board == hot.hilite) {
			draw::drag::begin(last_board);
			camera_drag = camera;
		} else
			return false;
		break;
	default:
		if(draw::drag::active(last_board)) {
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (draw::drag::mouse - hot.mouse);
			return true;
		}
		return false;
	}
	return true;
}

void control_standart() {
	if(control_focus())
		return;
	if(control_board())
		return;
}

static void draw_icon(int& x, int& y, int x0, int x2, int* max_width, int& w, const char* id) {
	static piclib source;
	auto& e = source.get(id, "icons", 0, 0);
	auto dy = draw::texth();
	w = e.width;
	if(x + w > x2) {
		if(max_width)
			*max_width = imax(*max_width, x - x0);
		x = x0;
		y += draw::texth();
	}
	draw::blit(*draw::canvas, x, y + dy - e.height - 2, w, e.height, ImageTransparent,
		e, 0, 0);
}

void draw::tooltips(int x1, int y1, int width, const char* format, ...) {
	tooltips_point.x = x1;
	tooltips_point.y = y1;
	tooltips_width = width;
	szprint(tooltips_text, tooltips_text + sizeof(tooltips_text) - 1, format, xva_start(format));
}

static void render_tooltips(const char* format, int x, int y, int w) {
	if(!format || !format[0])
		return;
	draw::state push;
	draw::font = metrics::font;
	if(!draw::font)
		return;
	rect rc;
	rc.x1 = x + w + gui.border * 2 + gui.padding;
	rc.y1 = y;
	rc.x2 = rc.x1 + gui.tips_width;
	rc.y2 = rc.y1;
	draw::textf(rc, format);
	if(rc.x2 > getwidth() - gui.border - gui.padding) {
		auto w = rc.width();
		rc.x1 = x - gui.border * 2 - gui.padding - w;
		rc.x2 = rc.x1 + w;
	}
	// Correct border
	int height = draw::getheight();
	int width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
	window(rc, false, false);
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), format);
}

static void render_tooltips() {
	render_tooltips(tooltips_text, tooltips_point.x, tooltips_point.y, tooltips_width);
	tooltips_text[0] = 0;
}

void draw::initialize() {
	colors::active = color::create(172, 128, 0);
	colors::border = color::create(73, 73, 80);
	colors::button = color::create(0, 122, 204);
	colors::form = color::create(32, 32, 32);
	colors::window = color::create(64, 64, 64);
	colors::text = color::create(255, 255, 255);
	colors::edit = color::create(38, 79, 120);
	colors::special = color::create(255, 244, 32);
	colors::border = colors::window.mix(colors::text, 128);
	colors::tips::text = color::create(255, 255, 255);
	colors::tips::back = color::create(100, 100, 120);
	colors::tabs::back = color::create(255, 204, 0);
	colors::tabs::text = colors::black;
	colors::h1 = colors::text.mix(colors::edit, 64);
	colors::h2 = colors::text.mix(colors::edit, 96);
	colors::h3 = colors::text.mix(colors::edit, 128);
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
	player_colors[0][0] = color::create(97, 189, 79);
	player_colors[1][0] = color::create(242, 214, 0);
	player_colors[2][0] = color::create(255, 159, 26);
	player_colors[3][0] = color::create(235, 90, 70);
	player_colors[4][0] = color::create(195, 119, 224);
	player_colors[5][0] = color::create(0, 121, 191);
	for(auto& e : player_colors)
		e[1] = e[0].mix(colors::black, 128);
	gui.initialize();
	set(draw_icon);
}

static bool read_sprite(sprite** result, const char* name) {
	if(*result)
		delete *result;
	char temp[260]; zprint(temp, "art/sprites/%1.pma", name);
	*result = (sprite*)loadb(temp);
	return (*result) != 0;
}

static void end_turn() {
	breakmodal(0);
}

void draw::domodal() {
	if(current_execute.proc) {
		auto ev = current_execute;
		before_render();
		hot.key = InputUpdate;
		hot.param = ev.param;
		ev.proc();
		before_render();
		hot.key = InputUpdate;
		return;
	}
	render_tooltips();
	if(hot.key == InputUpdate && keep_hot) {
		keep_hot = false;
		hot = keep_hot_value;
	} else
		hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
}

const int size2 = size - 4;
static const point hexagon_offset2[6] = {{(short)(size2 * cos_30), -(short)(size2 / 2)},
{(short)(size2 * cos_30), (short)(size2 / 2)},
{0, size2},
{-(short)(size2 * cos_30), (short)(size2 / 2)},
{-(short)(size2 * cos_30), -(short)(size2 / 2)},
{0, -size2},
};
static const point hexagon_offset[6] = {{(short)(size * cos_30), -(short)(size / 2)},
{(short)(size * cos_30), (short)(size / 2)},
{0, size},
{-(short)(size * cos_30), (short)(size / 2)},
{-(short)(size * cos_30), -(short)(size / 2)},
{0, -size},
};

static const point planets_n2[] = {{(short)(size / 2), (short)(-size / 4)},
{(short)(-size / 2), (short)(size / 4)}
};
static const point planets_n3[] = {{0, (short)(-size / 2)},
{(short)(-size / 2), 0},
{(short)(size / 3), (short)(size / 3)}
};

static point h2p(point hex) {
	short x = short(size * sqrt_3) * hex.x + (short(size * sqrt_3) / 2) * hex.y;
	short y = size * 3 / 2 * hex.y;
	return {x, y};
}

static cube cube_round(cube c) {
	double rx = int(c.x);
	double ry = int(c.y);
	double rz = int(c.z);
	auto x_diff = iabs((double)rx - c.x);
	auto y_diff = iabs((double)ry - c.y);
	auto z_diff = iabs((double)rz - c.z);
	if(x_diff > y_diff && x_diff > z_diff)
		rx = -ry - rz;
	else if(y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;
	return {rx, ry, rz};
}

static point cube_to_oddr(cube c) {
	auto col = c.x + (c.z - (((int)c.z) & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static point flat_hex_to_pixel(point hex) {
	auto x = (short)((size * 3.0 * hex.x) / 2);
	auto y = (short)(size * sqrt_3 * hex.x + 0.5 * (hex.y & 1));
	return {x, y};
}

static point pixel_to_flat_hex(point pixel) {
	auto q = (short)(((2.0 / 3.0) * pixel.x) / size);
	auto r = (short)(((-1.0 / 3.0) * pixel.x + sqrt_3 / 3 * pixel.y) / size);
	return {q, r};
}

point hex_round(point pt) {
	return pt;
}

static cube axial_to_cube(point pt) {
	return {0, 0, 0};
}

static point cube_to_axial(cube c) {
	return {0, 0};
}

static point p2h(point pt) {
	auto q = ((sqrt_3 / 3.0) * (double)pt.x - (1.0 / 3.0) * (double)pt.y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_oddr(cube_round(axial_to_cube({(short)q, (short)r})));
}

static void hexagon2(point pt) {
	auto push_linw = linw;
	auto push_fore = fore;
	linw = 2.0;
	fore = colors::red;
	for(auto i = 0; i < 5; i++) {
		auto p1 = pt + hexagon_offset2[i];
		auto p2 = pt + hexagon_offset2[i + 1];
		line(p1.x, p1.y, p2.x, p2.y);
	}
	auto p1 = pt + hexagon_offset2[5];
	auto p2 = pt + hexagon_offset2[0];
	line(p1.x, p1.y, p2.x, p2.y);
	linw = push_linw;
	fore = push_fore;
}

static void hexagon(point pt) {
	for(auto i = 0; i < 5; i++)
		draw::line(pt + hexagon_offset[i], pt + hexagon_offset[i + 1], colors::border);
	draw::line(pt + hexagon_offset[5], pt + hexagon_offset[0], colors::border);
}

static void trianglef(int x1, int y1, int dy, int dx, color c1) {
	auto d = dx / 2;
	if(!dy)
		return;
	auto push_fore = fore;
	fore = c1;
	for(auto y = 0; y < dy; y++) {
		auto x = dx * y / dy;
		line(x1 - x, y1 + y, x1 + x, y1 + y);
	}
	fore = push_fore;
}

static void triangleb(int x1, int y1, int dy, int dx, color c1) {
	auto push_fore = fore;
	fore = c1;
	line(x1, y1, x1 - dx, y1 + dy);
	line(x1, y1, x1 + dx, y1 + dy);
	line(x1 - dx, y1 + dy, x1 + dx, y1 + dy);
	fore = push_fore;
}

static void cannon(int x, int y, color c1, color c2) {
	line(x, y, x + unit_size / 2, y, c1); y++;
	line(x, y, x + unit_size / 2, y, c2);
}

static void draw_unit(int x, int y, group_s type, int count, color c1, color c2) {
	int n;
	char temp[32]; zprint(temp, "%1i", count);
	const int r = unit_size;
	auto show_count = true;
	switch(type) {
	case Fighters:
		circlef(x, y, r, c1);
		circle(x, y, r, c2);
		break;
	case GroundForces:
		rectf({x - r, y - r, x + r, y + r}, c1);
		rectb({x - r, y - r, x + r, y + r}, c2);
		break;
	case PDS:
		x += r;
		trianglef(x, y - r, r * 2, r, c1);
		triangleb(x, y - r, r * 2, r, c2);
		break;
	case SpaceDock:
		show_count = false;
		circlef(x, y, r * 3, c1, 128);
		circle(x, y, r * 3, c2);
		break;
	case Carrier:
		zcat(temp, "Т");
		n = r + r;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y, c1, c2);
		break;
	case Destroyer:
		zcat(temp, "Э");
		n = r + r / 2;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 2, c1, c2);
		cannon(x + n, y + 2, c1, c2);
		break;
	case Cruiser:
		zcat(temp, "К");
		n = r + r / 2;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 3, c1, c2);
		cannon(x + n, y, c1, c2);
		cannon(x + n, y + 3, c1, c2);
		break;
	case Dreadnought:
		zcat(temp, "Л");
		n = r + r;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 6, c1, c2);
		cannon(x + n, y - 3, c1, c2);
		cannon(x + n, y, c1, c2);
		cannon(x + n, y + 3, c1, c2);
		break;
	case WarSun:
		zcat(temp, "ВС");
		circlef(x + r, y + r, r * 2, c1);
		circle(x + r, y + r, r * 2, c2);
		break;
	}
	if(show_count) {
		auto push_font = font;
		font = font_small;
		text(x - textw(temp) / 2 - 1, y - texth() / 2, temp);
		font = push_font;
	}
}

static int compare_units(const void* v1, const void* v2) {
	auto e1 = *((uniti**)v1);
	auto e2 = *((uniti**)v2);
	return (int)e1->type - (int)e2->type;
}

static void draw_units(int x, int y, uniti* parent, bool ground) {
	struct unit_draw_info {
		group_s		type;
		char			count;
		void clear() {
			type = NoUnit;
			count = 0;
		}
	};
	auto parent_player = parent->getplayer();
	if(!parent_player)
		return;
	auto player_index = parent_player->getid();
	auto c1 = player_colors[player_index][0];
	auto c2 = player_colors[player_index][1];
	adat<uniti*, 32> source;
	source.count = uniti::select(source.begin(), source.endof(), parent);
	if(!source) {
		if(ground) {
			circlef(x, y, unit_size, c1, 128);
			circle(x, y, unit_size, c2);
		}
		return;
	}
	qsort(source.data, source.count, sizeof(source.data[0]), compare_units);
	// Stardock draw separately
	if(source.data[0]->type == SpaceDock) {
		draw_unit(x, y, SpaceDock, 1, c1, c2);
		source.remove(0);
		if(!source)
			return;
	}
	// Remove unique
	adat<unit_draw_info, 8> drawing;
	memset(drawing.data, 0, sizeof(drawing.data));
	auto pd = drawing.data;
	for(unsigned i = 0; i < source.count; i++) {
		if(pd->type == source.data[i]->type)
			pd->count++;
		else {
			if(pd->type != NoUnit)
				pd++;
			pd->type = source.data[i]->type;
			pd->count = 1;
		}
	}
	drawing.count = (pd - drawing.data) + 1;
	// Draw
	if(ground) {
		auto x0 = x - ((unit_size * 2 + 2) * drawing.count) / 2 + (unit_size * 2 + 2) / 2;
		for(unsigned i = 0; i < drawing.count; i++) {
			draw_unit(x0, y, drawing.data[i].type, drawing.data[i].count, c1, c2);
			x0 += unit_size + 2;
		}
	} else {
		for(unsigned i = 0; i < drawing.count; i++) {
			draw_unit(x, y, drawing.data[i].type, drawing.data[i].count, c1, c2);
			y += unit_size * 2 + 2;
		}
	}
}

static void draw_planet(point pt, planeti* p) {
	auto push_font = font;
	auto push_stro = fore_stroke;
	fore_stroke = colors::black;
	font = metrics::h1;
	image(pt.x, pt.y, planets, p->index, 0);
	auto pn = p->name;
	text(pt.x - textw(pn) / 2, pt.y + 128 / 2, pn, -1, TextStroke);
	fore_stroke = push_stro;
	font = push_font;
	draw_units(pt.x, pt.y, p, true);
}

static int render_left() {
	int x = gui.border;
	int y = gui.border;
	int current_y, current_w;
	for(auto& e : bsmeta<playeri>()) {
		areas a = AreaNormal;
		auto w = render_picture(x, y, e.id, &a);
		rect rc = {x, y, x + w, y + w};
		if(e.isactive()) {
			current_y = y;
			current_w = w;
		}
		if(e.gethuman() == &e) {
			rectb(rc, colors::white);
			rc.offset(-1, -1);
		}
		if(e.getspeaker() == &e) {
			rectb(rc, colors::blue); rc.offset(-1, -1);
			rectb(rc, colors::blue.mix(colors::form)); rc.offset(-1, -1);
		}
		if(a == AreaHilited || a == AreaHilitedPressed) {
			string sb;
			e.getinfo(sb);
			tooltips(x - gui.border, y + gui.border, w, sb);
			hilited = &e;
		}
		y += w + gui.padding;
	}
	auto active = playeri::getactive();
	if(active && hilited.type != Player) {
		string sb;
		active->getinfo(sb);
		render_tooltips(sb, x - gui.border, current_y + gui.border, current_w);
	}
	return y;
}

static void render_board(bool use_hilite_solar = false, bool show_movement = false) {
	last_board = {0, 0, getwidth(), getheight()};
	rectf(last_board, colors::window);
	area(last_board);
	hilited.clear();
	for(auto y = 0; y < 8; y++) {
		for(auto x = 0; x < 8; x++) {
			auto pt = h2p({(short)x, (short)y}) - camera;
			rect rcp = {pt.x - size, pt.y - size, pt.x + size, pt.y + size};
			if(rcp.x2<last_board.x1 || rcp.y2<last_board.y1
				|| rcp.x1 > last_board.x2 || rcp.y1 > last_board.y2)
				continue;
			auto index = planeti::gmi(x, y);
			auto p = uniti::getsolar(index);
			if(!p)
				continue;
			hexagon(pt);
			if(use_hilite_solar) {
				auto dx = size2 / 2;
				rect rc = {pt.x - dx, pt.y - dx, pt.x + dx, pt.y + dx};
				if(areb(rc))
					hilited = p;
			}
			if(hilited == p)
				hexagon2(pt);
			if(p->type == SolarSystem) {
				adat<planeti*, 3> source;
				source.count = planeti::select(source.begin(), source.endof(), p);
				switch(source.count) {
				case 0:
					break;
				case 1:
					draw_planet(pt, source[0]);
					break;
				case 2:
					draw_planet(pt + planets_n2[0], source[0]);
					draw_planet(pt + planets_n2[1], source[1]);
					break;
				case 3:
					draw_planet(pt + planets_n3[0], source[0]);
					draw_planet(pt + planets_n3[1], source[1]);
					draw_planet(pt + planets_n3[2], source[2]);
					break;
				}
			} else if(p->type == AsteroidField)
				image(pt.x, pt.y, planets, 19, 0);
			else if(p->type == Nebula)
				image(pt.x, pt.y, planets, 17, 0);
			else if(p->type == Supernova)
				image(pt.x, pt.y, planets, 18, 0);
			draw_units(pt.x - size / 3, pt.y - size / 3, p, false);
			if(show_movement) {
				auto push_font = font;
				auto value = uniti::getmovement(index);
				if(value && value != Blocked) {
					font = metrics::h1;
					char temp[16]; zprint(temp, "%1i", value);
					font = push_font;
					text(pt.x - textw(temp), pt.y - texth() / 2, temp, -1, TextStroke);
				}
			}
		}
	}
}

static int render_report(int x, int y, const char* picture, const char* format) {
	if(!format)
		return 0;
	auto y0 = y;
	if(picture)
		y += window(x, y, gui.hero_width, gui.window_width, picture, format);
	else
		y += window(x, y, gui.window_width, format, gui.window_width);
	y += gui.padding;
	return y - y0;
}

int	answeri::choosev(bool cancel_button, tips_proc tips, const char* picture, const char* format) const {
	int x, y;
	if(cancel_button && !elements)
		return 0;
	while(ismodal()) {
		render_board();
		render_left();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		y += render_report(x, y, picture, format);
		x = getwidth() - gui.right_width - gui.border * 2;
		for(auto& e : elements) {
			bool run;
			y += windowb(x, y, gui.right_width, e.getname(), run, false);
			if(run)
				execute(breakparam, e.param);
		}
		if(cancel_button)
			y += windowb(x, y, gui.right_width, "cancel", buttoncancel, false, 0, KeyEscape);
		domodal();
		control_standart();
	}
	return getresult();
}

struct unit_ref_table : table {
	army&		source;
	bool		choosed;
	const char* getname(char* result, const char* result_maximum, int line, int column) const override {
		if(columns[column] == "name")
			return source[line]->getname();
		return 0;
	}
	int getnumber(int line, int column) const override {
		return 0;
	}
	uniti* getvalue() const {
		return source[current];
	}
	int	getmaximum() const override {
		return source.getcount();
	}
	bool keyinput(unsigned id) override {
		switch(id) {
		case KeyEnter:
			if(getmaximum() > 0)
				choosed = true;
			break;
		default: return table::keyinput(id);
		}
		return true;
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "Наименование", 176},
		{}};
		return columns;
	}
	static int compare(const void* p1, const void* p2) {
		auto u1 = *((uniti**)p1);
		auto u2 = *((uniti**)p2);
		return strcmp(u1->getname(), u2->getname());
	}
	void view(const rect& rc) override {
		choosed = false;
		table::view(rc);
	}
	constexpr unit_ref_table(army& source) : table(getcolumns()), source(source), choosed(false) {}
};

struct unit_table : table {
	struct element {
		uniti	unit;
		int			count;
	};
	static const int table_maximum = (WarSun - GroundForces + 1);
	adat<element, table_maximum> source;
	bool			focusable;
	int				fleet, fleet_used, resource, maximal;
	int				total_fleet, total_resource, total_maximal;
	int getmaximum() const override {
		return source.getcount();
	}
	const char* getname(char* result, const char* result_maximum, int line, int column) const override {
		if(columns[column] == "name")
			return getstr(source[line].unit.type);
		return 0;
	}
	int getnumber(int line, int column) const override {
		if(columns[column] == "resource")
			return source[line].unit.getresource();
		if(columns[column] == "count")
			return source[line].count;
		if(columns[column] == "count_units")
			return source[line].count * uniti::getproduction(source[line].unit.type);
		if(columns[column] == "total")
			return source[line].unit.getresource()*source[line].count;
		if(columns[column] == "fleet")
			return source[line].unit.isfleet() ? source[line].count * 1 : 0;
		return 0;
	}
	group_s getvalue() const {
		return source[current].unit.type;
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "Наименование", 176},
		{Number | AlignRight, "resource", "Цена", 32},
		{Number | AlignRight, "count_units", "К-во", 32},
		{Number | AlignRight, "total", "Сумма", 48},
		{Number | AlignRight, "fleet", "Флот", 48},
		{}};
		return columns;
	}
	static int compare(const void* p1, const void* p2) {
		auto i1 = *((group_s*)p1);
		auto i2 = *((group_s*)p2);
		return strcmp(getstr(i1), getstr(i2));
	}
	static void add_value() {
		auto p = (unit_table*)hot.param;
		auto i = p->current;
		p->source[i].count++;
	}
	static void sub_value() {
		auto p = (unit_table*)hot.param;
		auto i = p->current;
		p->source[i].count--;
	}
	void row(const rect &rc, int index) override {
		table::row(rc, index);
		bool disabled;
		auto x = rc.x2 - 2;
		auto y1 = rc.y1 + 1;
		auto y2 = rc.y2 - 2;
		auto h = y2 - y1;
		auto focused = (index == current);
		disabled = false;
		if(total_maximal >= maximal)
			disabled = true;
		if(total_resource >= resource)
			disabled = true;
		if(source[index].unit.isfleet() && total_fleet >= fleet)
			disabled = true;
		if(buttonh({x - h, y1, x, y2}, false, focused, disabled, true, "+", Alpha + '+', true)) {
			execute(add_value, (int)this);
		}
		x -= h + 2;
		disabled = false;
		if(source[index].count <= 0)
			disabled = true;
		if(buttonh({x - h, y1, x, y2}, false, focused, disabled, true, "-", Alpha + '-', true)) {
			execute(sub_value, (int)this);
		}
	}
	void view(const rect& rc) {
		total_maximal = gettotal("count_units");
		total_resource = gettotal("total");
		total_fleet = gettotal("fleet") + fleet_used;
		table::view(rc);
		rect rv = {rc.x1, rc.y2 - getrowheight(), rc.x2, rc.y2};
		string sb;
		sb.add("Ваши ресурсы [%1i], флот [%4i]/[%2i], продукция [%3i]", resource, fleet, maximal, total_fleet);
		textf(rv.x1 + 4, rv.y1 + 4, rv.width(), sb);
	}
	unit_table(playeri* player) : table(getcolumns()), fleet(-1), resource(-1), maximal(0) {
		memset(source.data, 0, sizeof(source.data));
		const auto i1 = GroundForces;
		for(auto i = i1; i <= WarSun; i = (group_s)(i + 1)) {
			if(!player->isallow(i))
				continue;
			auto p = source.add();
			p->unit.type = i;
			p->unit.setplayer(player);
		}
	}
};

void playeri::slide(int x, int y) {
	const auto step = 16;
	auto x0 = camera.x;
	auto y0 = camera.y;
	auto w = last_board.width();
	if(!w)
		w = getwidth();
	auto h = last_board.width();
	if(!h)
		h = getheight();
	auto x1 = x - w / 2;
	auto y1 = y - h / 2;
	auto lenght = distance({(short)x0, (short)y0}, {(short)x1, (short)y1});
	if(!lenght)
		return;
	auto start = 0;
	auto dx = x1 - x0;
	auto dy = y1 - y0;
	while(start < lenght && ismodal()) {
		render_board();
		sysredraw();
		start += step;
		short x2 = x0 + dx * start / lenght;
		short y2 = y0 + dy * start / lenght;
		camera.x = x2;
		camera.y = y2;
	}
	camera.x = x1;
	camera.y = y1;
}

solari* playeri::choose_solar() const {
	while(ismodal()) {
		render_board(true);
		render_left();
		auto x = getwidth() - gui.window_width - gui.border * 2;
		auto y = gui.border * 2;
		y += window(x, y, gui.window_width, "Выбирайте звездную систему", gui.window_width);
		x = getwidth() - gui.right_width - gui.border * 2;
		domodal();
		control_standart();
		if(hot.pressed && hot.key == MouseLeft) {
			if(hilited.type == Solar)
				breakmodal((int)hilited.getsolar());
		}
	}
	auto p = reinterpret_cast<solari*>(getresult());
	return p;
}

void playeri::slide(int hexagon) {
	if(hexagon == -1)
		return;
	auto x = uniti::gmx(hexagon);
	auto y = uniti::gmy(hexagon);
	point pt = h2p({(short)x, (short)y});
	slide(pt.x, pt.y);
}

bool playeri::build(army& units, const planeti* planet, solari* system, int resources, int fleet, int minimal, int maximal, bool cancel_button) {
	int x, y;
	unit_table u1(this);
	u1.fleet = fleet;
	u1.fleet_used = system->getfleet(this, system);
	u1.resource = resources;
	u1.maximal = maximal;
	auto text_width = gui.window_width;
	slide(system);
	while(ismodal()) {
		render_board();
		render_left();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		rect rc = {x, y, x + gui.window_width, y + u1.getrowheight()*(u1.getmaximum() + 2) + 1};
		window(rc, false, false);
		u1.view(rc);
		x = getwidth() - gui.right_width - gui.border * 2;
		y += rc.height() + gui.padding + gui.border * 2;
		y += windowb(x, y, gui.right_width, "Построить", buttonok, false, 0, KeyEnter);
		if(cancel_button)
			y += windowb(x, y, gui.right_width, "Отмена", buttoncancel, false, 0, KeyEscape);
		domodal();
		control_standart();
	}
	auto result = getresult() != 0;
	if(result) {
		for(auto& e : u1.source) {
			for(auto i = e.count * uniti::getproduction(e.unit.type); i > 0; i--) {
				if(e.unit.isplanetary())
					create(e.unit.type, const_cast<planeti*>(planet));
				else
					create(e.unit.type, system);
			}
		}
	}
	return result;
}

bool playeri::choose(army& a1, army& a2, const char* action, bool cancel_button, bool show_movement) const {
	int x, y;
	unit_ref_table u1(a1); u1.show_header = false;
	unit_ref_table u2(a2); u2.show_header = false;
	auto maximum = imax(u1.getmaximum(), u2.getmaximum()) + 1;
	while(ismodal()) {
		render_board(false, show_movement);
		render_left();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		rect rc = {x, y, x + gui.window_width, y + u1.getrowheight()*maximum + 1};
		auto w2 = rc.width() / 2 - gui.padding / 2;
		rect rc1 = {rc.x1, rc.y1, rc.x1 + w2, rc.y2};
		rect rc2 = {rc1.x2 + gui.padding, rc.y1, rc.x2, rc.y2};
		window(rc, false, false);
		u1.view(rc1);
		u2.view(rc2);
		x = getwidth() - gui.right_width - gui.border * 2;
		y += rc.height() + gui.padding + gui.border * 2;
		y += windowb(x, y, gui.right_width, action, buttonok, false, 0, KeyEnter);
		if(cancel_button)
			y += windowb(x, y, gui.right_width, "Отмена", buttoncancel, false, 0, KeyEscape);
		domodal();
		control_standart();
		if(u1.choosed) {
			auto p = u1.getvalue();
			a1.remove(u1.current);
			a2.add(p);
		} else if(u2.choosed) {
			auto p = u2.getvalue();
			a2.remove(u2.current);
			a1.add(p);
		}
	}
	return getresult() != 0;
}

bool playeri::choose_movement(uniti* solar) const {
	while(ismodal()) {
		render_board(false, true);
		render_left();
		auto x = getwidth() - gui.window_width - gui.border * 2;
		auto y = gui.border * 2;
		x = getwidth() - gui.right_width - gui.border * 2;
		domodal();
		control_standart();
	}
	auto p = reinterpret_cast<uniti*>(getresult());
	return p;
}