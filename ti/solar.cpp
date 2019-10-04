#include "main.h"

const char*	solari::getname() const {
	auto p = getplanet(1);
	if(p)
		return p->getname();
	return getstr(type);
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

void solari::setplayer(const playeri* v) {
	if(v)
		player = v - bsmeta<playeri>::elements;
	else
		player = 0xFF;
}

int	solari::getfleet(const playeri* player) const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getsolar() != this)
			continue;
		if(!e.isfleet())
			continue;
		result++;
	}
	return result;
}

int	solari::getfleetsupport(const playeri* player) const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
			continue;
		if(e.getsolar() != this)
			continue;
		auto capacity_count = e.getcapacity();
		if(!capacity_count)
			continue;
		result += capacity_count;
	}
	return result;
}

void solari::activate(const playeri* v) {
	if(!isactivated(v)) {
		flags |= 1 << (v->getid());
		v->slide(getindex());
	}
}

int	solari::getcount(variant_s type, const playeri* player) const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.type == type && e.getplayer() == player && e.getsolar()==this)
			result++;
	}
	return result;
}

void solari::select(planeta& result, unsigned flags) const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.getsolar() != this)
			continue;
		result.add(&e);
		if(flags&FirstEntry)
			break;
	}
}

solari* solari::getmekatol() {
	return bsmeta<solari>::elements;
}