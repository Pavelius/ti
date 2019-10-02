#include "main.h"

const char*	solari::getname() const {
	auto p = getplanet(1);
	if(p)
		return p->getname();
	return getstr(type);
}

void solari::select(solara& source, const playeri* player, bool include_mekatol_rex) {
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		if(player && e.getplayer() != player)
			continue;
		if(!include_mekatol_rex && e.ismekatol())
			continue;
		source.add(&e);
	}
}

bool solari::ismekatol() const {
	return this == bsmeta<solari>::elements;
}

planeti* solari::getplanet(int index) const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.getsolar() != this)
			continue;
		if(--index <= 0)
			return &e;
	}
	return 0;
}