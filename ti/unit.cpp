#include "main.h"

DECLBASE(uniti, 32 * 6);

void* uniti::operator new(unsigned size) {
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			return &e;
	}
	return bsmeta<uniti>::add();
}

uniti::~uniti() {
	type = NoVariant;
}

bool uniti::is(tech_s v) const {
	auto p = getplayer();
	if(!p)
		return false;
	return p->is(v);
}

bool uniti::is(bonus_s v) const {
	auto p = getplayer();
	if(!p)
		return false;
	return p->is(v);
}

int uniti::getmovement() const {
	auto result = getgroup().movements;
	switch(type) {
	case Carrier:
		if(is(XRDTransporters))
			result++;
		break;
	case Cruiser:
		if(is(TypeIVDrive))
			result++;
		break;
	case Dreadnought:
		if(is(TypeIVDrive))
			result++;
		break;
	case Fighters:
		if(is(AdvancedFighters))
			result = 2;
		break;
	}
	return result;
}

int	uniti::getresource() const {
	auto result = getgroup().cost;
	switch(type) {
	case Dreadnought:
		if(is(BonusCostDreadnought))
			result--;
		break;
	}
	return result;
}

int	uniti::getproduce() const {
	if(!parent)
		return 0;
	auto result = getproduce(type);
	if(!result)
		return 0;
	auto planet = parent.getplanet();
	if(planet)
		result += planet->getresource();
	return result;
}

int	uniti::getproduce(variant_s type) {
	return bsmeta<varianti>::elements[type].production_count;
}

int	uniti::getproduction(variant_s type) {
	return bsmeta<varianti>::elements[type].production;
}

int uniti::getmaxhits() const {
	switch(type) {
	case Dreadnought:
	case WarSun:
		return 2;
	default:
		return 1;
	}
}

weaponi uniti::getweapon() const {
	auto w = getgroup().combat;
	if(is(CombatBonusAll))
		w.bonus++;
	if(is(CombatPenalty))
		w.bonus--;
	if(type == Dreadnought && is(CombatBonusDreadnought))
		w.bonus++;
	if(type == Fighters && is(CombatBonusFighters))
		w.bonus++;
	switch(type) {
	case Fighters:
		if(is(Cybernetics))
			w.bonus++;
		if(is(AdvancedFighters))
			w.bonus++;
		break;
	case GroundForces:
		if(is(GenSynthesis))
			w.bonus++;
		break;
	case Cruiser:
	case Destroyer:
		if(is(HylarVAssaultLaser))
			w.bonus++;
		break;
	case PDS:
		if(is(MagenDefenseGrid))
			w.bonus++;
		if(is(GravitonLaserSystem))
			w.reroll++;
		break;
	}
	return w;
}

weaponi uniti::getweapon(bool attacker, const playeri* opponent, char round) const {
	auto w = getweapon();
	if(type == GroundForces && attacker && is(CombatBonusGroundForcesAttack))
		w.bonus++;
	if(attacker && round == 1 && opponent->is(CombatBonusDefend))
		w.bonus--;
	return w;
}

bool uniti::isfleet() const {
	switch(type) {
	case Carrier:
	case Cruiser:
	case Destroyer:
	case Dreadnought:
	case WarSun:
		return true;
	default:
		return false;
	}
}

bool uniti::isinvaders() const {
	switch(type) {
	case GroundForces:
		return true;
	case Fighters:
		return is(GravitonNegator);
	default:
		return false;
	}
}

bool uniti::isplanetary(variant_s type) {
	switch(type) {
	case GroundForces:
	case PDS:
	case SpaceDock:
		return true;
	default:
		return false;
	}
}

int	uniti::getcount() const {
	return getgroup().count;
}

int	uniti::getcapacity() const {
	switch(type) {
	case Carrier:
	case WarSun:
		return 6;
	case Dreadnought:
	case Cruiser:
		if(is(StasisCapsules))
			return 1;
		return 0;
	default:
		return 0;
	}
}

variant_s uniti::getcapacitylimit() const {
	switch(type) {
	case Dreadnought:
	case Cruiser:
		if(is(StasisCapsules))
			return GroundForces;
		return NoVariant;
	default:
		return NoVariant;
	}
}

int	uniti::getcarried() const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.parent == this)
			result++;
	}
	return result;
}

int uniti::getjoincount(variant_s object) const {
	auto maximum = getcapacity();
	if(!maximum)
		return 0;
	auto limit = getcapacitylimit();
	if(limit && object != limit)
		return 0;
	auto current = getcarried();
	return maximum - current;
}

bool uniti::build(variant_s object, bool run) {
	auto solar = getsolar();
	if(!solar)
		return false;
	auto produce_count = getproduction(object);
	auto available_count = bsmeta<varianti>::elements[object].available;
	if(available_count) {
		auto exist_count = solar->getcount(object, getplayer());
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	auto planet = getplanet();
	if(!planet)
		return false;
	if(planet->getplayer() != getplayer())
		return false;
	if(object == Fighters && !is(AdvancedFighters)) {
		auto available_count = solar->getfleetsupport(getplayer());
		//auto exist_count = getcount(object, getplayer(), solar);
		//if(exist_count + produce_count > available_count)
		//	produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	if(run) {
		for(auto i = 0; i < produce_count; i++) {
			auto p = new uniti(object);
			p->setplanet(planet);
			p->setplayer(getplayer());
		}
	}
	return true;
}

int uniti::getweight() const {
	auto result = getresource() * 2;
	auto build_count = getproduction(type);
	if(build_count)
		result = result / build_count;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.parent == this)
			result += e.getweight();
	}
	return result;
}

void uniti::destroy() {}

playeri* uniti::getplayer() const {
	if(player == 0xFF)
		return 0;
	return &bsmeta<playeri>::elements[player];
}

void uniti::setplayer(const playeri* v) {
	if(!v)
		player = 0xFF;
	else
		player = v - bsmeta<playeri>::elements;
}

void uniti::setplanet(const planeti* v) {
	parent = const_cast<planeti*>(v);
}

void uniti::setsolar(const solari* v) {
	parent = const_cast<solari*>(v);
}

solari* uniti::getsolar() const {
	if(parent.type == Solar)
		return parent.getsolar();
	return 0;
}