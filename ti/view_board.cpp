#include "view.h"

const int hex_size = 128;
const double sqrt_3 = 1.732050807568877;

point draw::h2p(point hex) {
	short x = short(hex_size * sqrt_3) * hex.x + (short(hex_size * sqrt_3) / 2) * hex.y;
	short y = hex_size * 3 / 2 * hex.y;
	return {x, y};
}
