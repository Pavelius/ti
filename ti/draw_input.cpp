#include "main.h"
#include "draw_control.h"

using namespace draw;
using namespace draw::controls;

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

static int windowp(int x, int y, int width_picture, int width_text, const char* picture, const char* string, areas* pa = 0) {
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

static void icon(int x, int y, unit_type_s type, int count) {
	char temp[32];
	const int r = 12;
	const int a = r / 6;
	const int h = r * 3 / 2;
	draw::state push;
	fore = colors::red;
	if(count > 1)
		zprint(temp, "%1i", count);
	else
		temp[0] = 0;
	switch(type) {
	case GroundForces:
		circlef(x, y, r, fore, 96);
		circle(x, y, r);
		break;
	case PDS:
		rectf({x - r, y - r, x + r, y + r}, fore, 96);
		rectb({x - r, y - r, x + r, y + r});
		break;
	case Fighters:
		circlef(x, y, r, fore, 96);
		circle(x, y, r);
		line(x + r, y - r + a, x + r, y + r - a);
		line(x + r + 1, y - r + a, x + r + 1, y + r - a);
		line(x - r, y - r + a, x - r, y + r - a);
		line(x - r - 1, y - r + a, x - r - 1, y + r - a);
		break;
	case Carrier:
		rectf({x - h, y - r, x + h, y + r}, fore, 96);
		rectb({x - h, y - r, x + h, y + r});
		break;
	}
	fore = colors::white;
	text(x - textw(temp) / 2, y - texth() / 2, temp);
}

static const point hexagon_offset[6] = {{(short)(size * cos_30), -(short)(size / 2)},
{(short)(size * cos_30), (short)(size / 2)},
{0, size},
{-(short)(size * cos_30), (short)(size / 2)},
{-(short)(size * cos_30), -(short)(size / 2)},
{0, -size},
};

static const point planets_n2[] = {{(short)(size / 4), (short)(-size / 3)},
{(short)(-size / 4), (short)(size / 3)}
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

static void draw_planet(point pt, int r, color c) {
	draw::circlef(pt.x, pt.y, r, c, 128);
	draw::circle(pt.x, pt.y, r, c);
}

static void draw_planet(point pt, int n, int m, int r, color c) {
	switch(m) {
	case 2:
		draw_planet(pt + planets_n2[n], r, c);
		break;
	}
}

static int render_right() {
	auto x = gui.border;
	auto y = gui.border;
	for(auto p : active_players) {
		areas a = AreaNormal;
		auto w = render_picture(x, y, p->getid(), &a);
		rect rc = {x, y, x + w, y + w};
		if(p->gethuman() == p) {
			rectb(rc, colors::white);
			rc.offset(-1, -1);
		}
		if(p->getspeaker() == p) {
			rectb(rc, colors::blue); rc.offset(-1, -1);
			rectb(rc, colors::blue.mix(colors::form)); rc.offset(-1, -1);
		}
		if(a == AreaHilited || a == AreaHilitedPressed) {
			string sb;
			p->getinfo(sb);
			tooltips(x - gui.border, y + gui.border, w, sb);
		}
		y += w + gui.padding;
	}
	return y;
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
			hexagon(pt);
			draw_planet(pt, 0, 2, 16, colors::blue);
			draw_planet(pt, 1, 2, 20, colors::blue);
		}
	}
}

struct control_player_table : table {
	adat<player_s, 6>		source;
	bool					focusable;
	int getmaximum() const override {
		return source.count;
	}
	const char* getname(char* result, const char* result_maximum, int line, int column) const override {
		if(columns[column] == "name")
			return getstr(source[line]);
		else if(columns[column] == "politic")
			return getstr(players[source[line]].strategy);
		return 0;
	}
	int getnumber(int line, int column) const override {
		if(columns[column] == "resource")
			return planet_info::get(source[line], &planet_info::getresource);
		else if(columns[column] == "influence")
			return planet_info::get(source[line], &planet_info::getinfluence);
		else if(columns[column] == "planet_count")
			return planet_info::get(source[line], &planet_info::getone);
		else if(columns[column] == "fleet")
			return players[source[line]].get(Fleet);
		else if(columns[column] == "command")
			return players[source[line]].get(Command);
		else if(columns[column] == "strategy")
			return players[source[line]].get(Strategy);
		return 0;
	}
	player_s getvalue() const {
		return source[current];
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "Наименование", 160},
		{Text, "politic", "Политика", 100},
		{Number | AlignRight, "resource", "Рс", 32},
		{Number | AlignRight, "influence", "Вл", 32},
		{Number | AlignRight, "planet_count", "Пл", 32},
		{Number | AlignRight, "fleet", "Флот", 32},
		{Number | AlignRight, "command", "Ком", 32},
		{Number | AlignRight, "strategy", "Стр", 32},
		{}};
		return columns;
	}
	static int compare(const void* p1, const void* p2) {
		auto i1 = *((player_s*)p1);
		auto i2 = *((player_s*)p2);
		auto d = players[i1].getinitiative() - players[i2].getinitiative();
		if(d == 0)
			return strcmp(getstr(i1), getstr(i2));
		return d;
	}
	void initialize() {
		source.clear();
		for(auto p : active_players) {
			source.add(p->getindex());
		}
		qsort(source.data, source.count, sizeof(source.data[0]), compare);
	}
	bool isfocusable() const {
		return focusable;
	}
	control_player_table() : table(getcolumns()), focusable(false) {
		initialize();
	}
};

static bool player_info_tips(int x, int y, int radius) {
	rect rc = {x - radius, y - radius, x + radius, y + radius};
	draw::circlef(x, y, radius, colors::form, 128);
	draw::circle(x, y, radius);
	return areb(rc);
}

static void show_statistic() {
	rect rc;
	control_player_table mv;
	mv.current = -1;
	rc.x1 = gui.border * 2;
	rc.y1 = gui.border * 2;
	rc.x2 = rc.x1 + mv.columns->gettotalwidth();
	rc.y2 = rc.y1 + mv.getrowheight() * (mv.source.getcount() + 1);
	window(rc, false, false);
	mv.show_border = false;
	mv.show_selection = false;
	mv.view(rc);
}

static int show_right_buttoms() {
	auto radius = 16;
	auto x = getwidth() - radius - gui.border;
	auto y = gui.padding + gui.border + radius;
	if(player_info_tips(x, y, radius))
		show_statistic();
	return y + radius * 2;
}

int	answer_info::choosev(bool cancel_button, tips_proc tips, const char* picture, const char* format) const {
	int x, y;
	while(ismodal()) {
		render_board();
		render_right();
		x = getwidth() - gui.window_width - gui.border * 2;
		y = gui.border * 2;
		if(format) {
			if(picture)
				y += windowp(x, y, gui.hero_width, gui.window_width, picture, format);
			else
				y += window(x, y, gui.window_width, format, gui.window_width);
			y += gui.padding;
		}
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