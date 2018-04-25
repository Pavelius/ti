#include "view.h"

const short hex_size = 192;
const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;

static const point hexagon_offset[6] = {{(short)(hex_size * cos_30), -(short)(hex_size / 2)},
{(short)(hex_size * cos_30), (short)(hex_size / 2)},
{0, hex_size},
{-(short)(hex_size * cos_30), (short)(hex_size / 2)},
{-(short)(hex_size * cos_30), -(short)(hex_size / 2)},
{0, -hex_size},
};

point draw::h2p(point hex) {
	short x = short(hex_size * sqrt_3) * hex.x + (short(hex_size * sqrt_3) / 2) * hex.y;
	short y = hex_size * 3 / 2 * hex.y;
	return {x, y};
}

void draw::hexagon(point pt) {
	for(auto i = 0; i<5; i++)
		draw::line(pt + hexagon_offset[i], pt + hexagon_offset[i + 1], colors::border);
	draw::line(pt + hexagon_offset[5], pt + hexagon_offset[0], colors::border);
}
