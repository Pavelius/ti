#include "main.h"
#include "draw_control.h"

using namespace draw;
using namespace draw::controls;
static sprite* planets = (sprite*)loadb("art/sprites/planets.pma");
static sprite* font_small = (sprite*)loadb("art/fonts/small.pma");
static color player_colors[sizeof(players)/ sizeof(players[0])][2];
const int unit_size = 12;

enum ui_command_s {
	NoUICommand, ChooseLeft, ChooseRight, ChooseList,
};
struct cmdid {
	callback_proc		proc;
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
	static amap<const char*, surface> avatars;
	auto p = avatars.find(id);
	if(!p) {
		p = avatars.add(id, surface());
		p->value.resize(gui.hero_width, gui.hero_width, 32, true);
		char temp[260];
		zprint(temp, "art/portraits/%1.bmp", id);
		surface e(temp, 0);
		if(e)
			blit(p->value, 0, 0, p->value.width, p->value.height, 0, e, 0, 0, e.width, e.height);
	}
	blit(*draw::canvas, x, y, gui.hero_width, gui.hero_width, 0, p->value, 0, 0);
	rect rc = {x, y, x + gui.hero_width, y + gui.hero_width};
	rectb(rc, colors::border);
	if(pa)
		*pa = area(rc);
	return gui.hero_width;
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

static int windowb(int x, int y, int width, const char* string, const runable& e, int border = 0, unsigned key = 0, const char* tips = 0) {
	draw::state push;
	draw::font = metrics::font;
	rect rc = {x, y, x + width, y + draw::texth()};
	auto ra = window(rc, e.isdisabled(), true, border);
	draw::text(rc, string, AlignCenterCenter);
	if((ra == AreaHilited || ra == AreaHilitedPressed) && tips)
		tooltips(x, y, rc.width(), tips);
	if(!e.isdisabled()
		&& ((ra == AreaHilitedPressed && hot.key == MouseLeft)
			|| (key && key == hot.key)))
		e.execute();
	return rc.height() + gui.border * 2;
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
			hot.cursor = CursorAll;
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
	static amap<const char*, draw::surface> source;
	auto p = source.find(id);
	if(!p) {
		char temp[260];
		p = source.add(id, surface());
		memset(p, 0, sizeof(*p));
		p->value.read(szurl(temp, "art/icons", id, "png"));
	}
	auto dy = draw::texth();
	w = p->value.width;
	if(x + w > x2) {
		if(max_width)
			*max_width = imax(*max_width, x - x0);
		x = x0;
		y += draw::texth();
	}
	draw::blit(*draw::canvas, x, y + dy - p->value.height - 2, w, p->value.height, ImageTransparent, p->value, 0, 0);
}

void draw::tooltips(int x1, int y1, int width, const char* format, ...) {
	tooltips_point.x = x1;
	tooltips_point.y = y1;
	tooltips_width = width;
	szprint(tooltips_text, tooltips_text + sizeof(tooltips_text) - 1, format, xva_start(format));
}

static void render_tooltips() {
	if(!tooltips_text[0])
		return;
	draw::state push;
	draw::font = metrics::font;
	if(draw::font) {
		rect rc;
		rc.x1 = tooltips_point.x + tooltips_width + gui.border * 2 + gui.padding;
		rc.y1 = tooltips_point.y;
		rc.x2 = rc.x1 + gui.tips_width;
		rc.y2 = rc.y1;
		draw::textf(rc, tooltips_text);
		if(rc.x2 > getwidth() - gui.border - gui.padding) {
			auto w = rc.width();
			rc.x1 = tooltips_point.x - gui.border * 2 - gui.padding - w;
			rc.x2 = rc.x1 + w;
		}
		// Correct border
		int height = draw::getheight();
		int width = draw::getwidth();
		if(rc.y2 >= height)
			rc.move(0, height - 2 - rc.y2);
		if(rc.x2 >= width)
			rc.move(width - 2 - rc.x2, 0);
		window(rc);
		draw::fore = colors::tips::text;
		draw::textf(rc.x1, rc.y1, rc.width(), tooltips_text);
	}
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
	char temp[260];
	if(*result)
		delete *result;
	*result = (sprite*)loadb(szurl(temp, "art/sprites", name, "pma"));
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

static void draw_unit(int x, int y, unit_type_s type, int count, color c1, color c2) {
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
		zcat(temp, "T");
		n = r + r / 2;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y, c1, c2);
		break;
	case Destroyer:
		zcat(temp, "D");
		n = r + r / 2;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 2, c1, c2);
		cannon(x + n, y + 2, c1, c2);
		break;
	case Cruiser:
		zcat(temp, "C");
		n = r + r / 2;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 3, c1, c2);
		cannon(x + n, y, c1, c2);
		cannon(x + n, y + 3, c1, c2);
		break;
	case Dreadnought:
		zcat(temp, "R");
		n = r + r;
		rectf({x - r, y - r, x + n, y + r}, c1);
		rectb({x - r, y - r, x + n, y + r}, c2);
		cannon(x + n, y - 6, c1, c2);
		cannon(x + n, y - 3, c1, c2);
		cannon(x + n, y, c1, c2);
		cannon(x + n, y + 3, c1, c2);
		break;
	case WarSun:
		zcat(temp, "W");
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
	auto e1 = *((unit_info**)v1);
	auto e2 = *((unit_info**)v2);
	return (int)e1->type - (int)e2->type;
}

static void draw_units(int x, int y, unit_info* parent, bool ground) {
	struct unit_draw_info {
		unit_type_s		type;
		char			count;
		void clear() {
			type = NoUnit;
			count = 0;
		}
	};
	auto player_index = parent->player->getindex();
	auto c1 = player_colors[player_index][0];
	auto c2 = player_colors[player_index][1];
	adat<unit_info*, 32> source;
	source.count = unit_info::select(source.begin(), source.endof(), parent);
	if(!source) {
		if(ground) {
			if(parent->player) {
				circlef(x, y, unit_size, c1, 128);
				circle(x, y, unit_size, c2);
			}
		}
		return;
	}
	qsort(source.data, source.count, sizeof(source.data[0]), compare_units);
	// Stardock draw separately
	if(source.data[0]->type == SpaceDock)
		draw_unit(x, y, SpaceDock, 1, c1, c2);
	source.remove(0);
	if(!source)
		return;
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

static void draw_planet(point pt, planet_info* p) {
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

static int render_right() {
	int x = gui.border;
	int y = gui.border;
	for(auto& e : players) {
		areas a = AreaNormal;
		auto w = render_picture(x, y, e.getid(), &a);
		rect rc = {x, y, x + w, y + w};
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
		}
		y += w + gui.padding;
	}
	return y;
}

static unit_info* get_system_index(int index) {
	auto n = solar_map[index];
	if(n == -1)
		return 0;
	return solars + n;
}

static void render_board() {
	last_board = {0, 0, getwidth(), getheight()};
	rectf(last_board, colors::window);
	area(last_board);
	for(auto y = 0; y < 8; y++) {
		for(auto x = 0; x < 8; x++) {
			auto pt = h2p({(short)x, (short)y}) - camera;
			rect rcp = {pt.x - size, pt.y - size, pt.x + size, pt.y + size};
			if(rcp.x2<last_board.x1 || rcp.y2<last_board.y1
				|| rcp.x1 > last_board.x2 || rcp.y1 > last_board.y2)
				continue;
			auto p = get_system_index(planet_info::gmi(x, y));
			if(!p)
				continue;
			hexagon(pt);
			if(p->type == SolarSystem) {
				adat<planet_info*, 3> source;
				source.count = planet_info::select(source.begin(), source.endof(), p);
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

int	answer_info::choosev(bool cancel_button, tips_proc tips, const char* picture, const char* format) const {
	int x, y;
	while(ismodal()) {
		render_board();
		render_right();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		y += render_report(x, y, picture, format);
		x = getwidth() - gui.right_width - gui.border * 2;
		for(auto& e : elements)
			y += windowb(x, y, gui.right_width, e.getname(), cmd(breakparam, e.param));
		if(cancel_button)
			y += windowb(x, y, gui.right_width, "cancel", cmd(buttoncancel), 0, KeyEscape);
		domodal();
		control_standart();
	}
	return getresult();
}

struct unit_table : table {
	struct element {
		unit_info	unit;
		int			count;
	};
	static const int table_maximum = (WarSun - GroundForces + 1);
	adat<element, table_maximum> source;
	bool			focusable;
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
		if(columns[column] == "total")
			return source[line].unit.getresource()*source[line].count;
		return 0;
	}
	unit_type_s getvalue() const {
		return source[current].unit.type;
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "������������", 224},
		{Number | AlignRight, "resource", "����", 32},
		{Number | AlignRight, "count", "�-��", 32},
		{Number | AlignRight, "total", "�����", 48},
		{}};
		return columns;
	}
	static int compare(const void* p1, const void* p2) {
		auto i1 = *((unit_type_s*)p1);
		auto i2 = *((unit_type_s*)p2);
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
		if(buttonh({x - h, y1, x, y2}, false, focused, disabled, true, "+", Alpha + '+', true)) {
			execute(add_value, (int)this);
		}
		x -= h + 2;
		disabled = (source[index].count <= 0);
		if(buttonh({x - h, y1, x, y2}, false, focused, disabled, true, "-", Alpha + '-', true)) {
			execute(sub_value, (int)this);
		}
	}
	unit_table(player_info* player) : table(getcolumns()) {
		memset(source.data, 0, sizeof(source.data));
		const auto i1 = GroundForces;
		for(auto i = i1; i <= WarSun; i = (unit_type_s)(i + 1)) {
			if(!player->isallow(i))
				continue;
			auto p = source.add();
			p->unit.type = i;
			p->unit.player = player;
		}
		//qsort(source, table_maximum, sizeof(source[0]), compare);
	}
};

bool player_info::build(army& units, const planet_info* planet, const planet_info* system, int minimal, bool cancel_button) {
	int x, y;
	unit_table u1(this);
	auto text_width = gui.window_width;
	while(ismodal()) {
		render_board();
		render_right();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		rect rc = {x, y, x + gui.window_width, y + u1.getrowheight()*(u1.getmaximum() + 1) + 1};
		window(rc, false, false);
		u1.view(rc);
		x = getwidth() - gui.right_width - gui.border * 2;
		y += rc.height() + gui.padding + gui.border * 2;
		y += windowb(x, y, gui.right_width, "���������", cmd(buttoncancel), 0, KeyEnter);
		if(cancel_button)
			y += windowb(x, y, gui.right_width, "������", cmd(buttoncancel), 0, KeyEscape);
		domodal();
		control_standart();
	}
	return getresult() != 0;
}