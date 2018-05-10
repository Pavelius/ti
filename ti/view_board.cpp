#include "view.h"

const short size = 128;
//const short size = 192;
const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;
static rect rc_board;
static point camera;

struct cube {
	double x, y, z;
};

static const point hexagon_offset[6] = {{(short)(size * cos_30), -(short)(size / 2)},
{(short)(size * cos_30), (short)(size / 2)},
{0, size},
{-(short)(size * cos_30), (short)(size / 2)},
{-(short)(size * cos_30), -(short)(size / 2)},
{0, -size},
};

point draw::h2p(point hex) {
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

point hex_round(point pt) {
	return pt;
}

static cube axial_to_cube(point pt) {
	return {0, 0, 0};
}

static point cube_to_axial(cube c) {
	return {0, 0};
}

point draw::p2h(point pt) {
	auto q = ((sqrt_3 / 3.0) * (double)pt.x - (1.0 / 3.0) * (double)pt.y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_oddr(cube_round(axial_to_cube({(short)q, (short)r})));
}

void draw::hexagon(point pt) {
	for(auto i = 0; i < 5; i++)
		draw::line(pt + hexagon_offset[i], pt + hexagon_offset[i + 1], colors::border);
	draw::line(pt + hexagon_offset[5], pt + hexagon_offset[0], colors::border);
}

void draw::board() {
	rc_board = {0, 0, getwidth(), getheight()};
	rectf(rc_board, colors::window);
	area(rc_board);
	for(auto y = 0; y < 8; y++) {
		for(auto x = 0; x < 8; x++) {
			auto pt = h2p({(short)x, (short)y}) + camera;
			hexagon(pt);
			char temp[64];
			szprints(temp, endofs(temp), "%1i, %2i", x, y);
			text(pt.x, pt.y, temp);
		}
	}
}

void draw::icon(int x, int y, unit_s type, int count) {
	char temp[32];
	const int r = 12;
	const int a = r/6;
	const int h = r*3/2;
	draw::state push;
	fore = colors::red;
	if(count > 1)
		szprints(temp, endofs(temp), "%1i", count);
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
	text(x - textw(temp) / 2, y - texth()/2, temp);
}

bool draw::boardkeys(int id) {
	const int dx = 16;
	switch(id) {
	case KeyLeft: camera.x -= dx; break;
	case KeyUp: camera.y -= dx; break;
	case KeyDown: camera.y += dx; break;
	case KeyRight: camera.x += dx; break;
	case MouseLeft:
		break;
	default:
		return false;
	}
	return true;
}