#include "main.h"

void varianta::match_activated(const playeri* player, bool value) {
	auto pb = data;
	for(auto v : *this) {
		auto solar = v.getsolar();
		if(!solar) {
			auto planet = v.getplanet();
			if(planet)
				solar = planet->getsolar();
		}
		if(!solar)
			continue;
		if(solar->isactivated(player) == value)
			*pb++ = v;
	}
	count = pb - data;
}

void varianta::match(const solari* solar, bool value) {
	auto pb = data;
	for(auto v : *this) {
		if(!v)
			continue;
		auto p = v.getsolar();
		if(p) {
			if(value) {
				if(p != solar)
					continue;
			} else {
				if(p == solar)
					continue;
			}
		}
		*pb++ = v;
	}
}

void varianta::add_solars(const playeri* player) {
	for(auto& e : bsdata<solari>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
			continue;
		add(&e);
	}
}

void varianta::add_planets(const playeri* player) {
	for(auto& e : bsdata<planeti>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
			continue;
		add(&e);
	}
}

void varianta::add_units(const playeri* player) {
	for(auto& e : bsdata<uniti>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
			continue;
		add(&e);
	}
}

void varianta::select(const playeri* player, unsigned flags) {
	switch(flags&TargetMask) {
	case Unit:
		add_units(player);
		if(flags&Neutral)
			add_units(0);
		break;
	case Planet:
		add_planets(player);
		if(flags&Neutral)
			add_planets(0);
		if(flags&NoMekatol)
			match(solari::getmekatol(), false);
		break;
	default:
		return;
	}
	if(flags&Activated)
		match_activated(player, true);
	if(flags&Ready)
		match_activated(player, false);
}

void varianta::sort(int (variant::*proc)() const) {
	const auto i2 = count_maximum - 1;
	const auto i1 = 0;
	for(int i = i2; i > i1; i--) {
		for(int j = i1; j < i; j++) {
			auto t1 = (data[j].*proc)();
			auto t2 = (data[j + 1].*proc)();
			if(t1 > t2)
				iswap(data[j], data[j + 1]);
		}
	}
}

void varianta::removecasualty(const playeri* player) {
	varianta temp;
	for(auto v : *this) {
		if(!v)
			continue;
		temp.add(v);
	}
	temp.sort(&variant::getweight);
	if(temp.data[0])
		temp.data[0].destroy();
}

void varianta::rollup() {
	auto ps = data;
	auto pe = data + count;
	for(auto pd = data; pd < pe; pd++) {
		auto found = false;
		if(!*pd)
			continue;
		for(auto pb = data; pb < ps; pb++) {
			if(*pb == *ps) {
				found = true;
				break;
			}
		}
		if(!found)
			*ps++ = *pd;
	}
}