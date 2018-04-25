#include "view.h"

const short size = 192;
const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;

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
	point hex_indecies[] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}};
	rectf({0, 0, getwidth(), getheight()}, colors::window);
	for(auto h : hex_indecies) {
		auto pt = h2p(h);
		pt = pt + 260;
		hexagon(pt);
		char temp[64];
		szprints(temp, endofs(temp), "%1i, %2i", h.x, h.y);
		text(pt.x, pt.y, temp);
	}
}