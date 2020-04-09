#include "main.h"

builda::builda(playeri* player) {
	memset(this, 0, sizeof(*this));
	const auto i1 = GroundForces;
	for(auto i = i1; i <= WarSun; i = (variant_s)(i + 1)) {
		if(!player->isallow(i))
			continue;
		auto p = add();
		p->settype(i);
		p->setplayer(player);
	}
}

unsigned char squad::getcount() const {
	return count*bsdata<varianti>::elements[gettype()].production;
}