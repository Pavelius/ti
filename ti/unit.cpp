#include "main.h"

static struct unit_info {
	const char*	id;
	const char*	name;
	char		available;
	char		cost;
	char		production;
	char		movements;
	weapon		combat;
} unit_data[] = {{""},
{"Solar", "Звездная система", 0, 0, 0, 0},
{"Nebula", "Небула", 0, 0, 0, 0},
{"Planet", "Планета", 0, 0, 0, 0},
{"SpaceDock", "Доки", 3, 4, 1, 0},
{"GroundForces", "Наземные силы", 0, 1, 2, 0, 8},
{"Fighters", "Истребители", 0, 1, 2, 0, 9},
{"PDS", "СПЗ", 6, 2, 1, 0, 6},
{"Carrier", "Транспорт", 4, 3, 1, 1, 9},
{"Cruiser", "Крейсер", 8, 2, 1, 2, 7},
{"Destroyer", "Эсминец", 8, 1, 1, 2, 9},
{"Dreadnought", "Дредноут", 5, 5, 1, 1, 5},
{"WarSun", "Звезда смерти", 2, 12, 1, 2, {3, 3}},
};
getstr_enum(unit);
adat<unit, 256>	units;

unsigned select(unit** result, unit** result_max, unit* location, player_s player, bool (unit::*test)() const) {
	auto p = result;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.player != player)
			continue;
		if(location && !e.in(location))
			continue;
		if(test && !(e.*test)())
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

unit* getminimal(unit** result, unsigned count, int (unit::*get)() const) {
	int value = 0;
	int index = -1;
	for(unsigned i = 0; i < count; i++) {
		auto r = (result[0]->*get)();
		if(index == -1 || r < value) {
			index = i;
			value = r;
		}
	}
	if(index == -1)
		return 0;
	return result[index];
}

void* unit::operator new(unsigned size) {
	for(auto& e : units) {
		if(!e)
			return &e;
	}
	return units.add();
}

unit::~unit() {
	type = NoUnit;
}

const char* unit::getname() const {
	return getstr(type);
}

int unit::getmovement() const {
	auto result = unit_data[type].movements;
	switch(type) {
	case Carrier:
		if(players[player].technologies.is(XRDTransporters))
			result++;
		break;
	case Cruiser:
		if(players[player].technologies.is(TypeIVDrive))
			result++;
		break;
	case Dreadnought:
		if(players[player].technologies.is(TypeIVDrive))
			result++;
		break;
	case Fighters:
		if(players[player].technologies.is(AdvancedFighters))
			result = 2;
		break;
	}
	return result;
}

int	unit::getresource() const {
	auto result = unit_data[type].cost;
	switch(player) {
	case TheL1z1xMindnet:
		if(type == Dreadnought)
			result--;
		break;
	}
	return result;
}

unit* unit::get(unit_s parent_type) {
	auto result = this;
	while(result && result->type != parent_type)
		result = result->parent;
	return result;
}

int	unit::getavailable(unit_s type) {
	return unit_data[type].available;
}

int	unit::getproduction(unit_s type) {
	return unit_data[type].production;
}

int unit::getmaxhits() const {
	switch(type) {
	case Dreadnought:
	case WarSun:
		return 2;
	default:
		return 1;
	}
}

weapon unit::getweapon() const {
	auto w = unit_data[type].combat;
	switch(player) {
	case SardakkNOrr:
		w.bonus++;
		break;
	case TheL1z1xMindnet:
		if(type == Dreadnought)
			w.bonus++;
		break;
	case TheNaaluCollective:
		if(type == Fighters)
			w.bonus++;
		break;
	case UniversitiesOfJolNar:
		w.bonus--;
		break;
	}
	auto& pi = players[player];
	switch(type) {
	case Fighters:
		if(pi.technologies.is(Cybernetics))
			w.bonus++;
		if(pi.technologies.is(AdvancedFighters))
			w.bonus++;
		break;
	case GroundForces:
		if(pi.technologies.is(GenSynthesis))
			w.bonus++;
		break;
	case Cruiser:
	case Destroyer:
		if(pi.technologies.is(HylarVAssaultLaser))
			w.bonus++;
		break;
	case PDS:
		if(pi.technologies.is(MagenDefenseGrid))
			w.bonus++;
		if(pi.technologies.is(GravitonLaserSystem))
			w.reroll++;
		break;
	}
	return w;
}

weapon unit::getweapon(bool attacker, player_s opponent, char round) const {
	auto w = getweapon();
	switch(player) {
	case TheL1z1xMindnet:
		if(type==GroundForces && attacker)
			w.bonus++;
		break;
	}
	if(opponent == TheXxchaKingdom && round == 1)
		w.bonus--;
	return w;
}

bool unit::isinvaders() const {
	switch(type) {
	case GroundForces:
		return true;
	case Fighters:
		return players[player].technologies.is(GravitonNegator);
	default:
		return false;
	}
}

bool unit::isplanetary(unit_s type) {
	switch(type) {
	case GroundForces:
	case PDS:
		return true;
	default:
		return false;
	}
}

int	unit::getcount(unit_s type, player_s player, unit* location) {
	auto result = 0;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.type == type && e.player == player)
			result++;
		if(location && !e.in(location))
			continue;
	}
	return result;
}

int	unit::getcapacity() const {
	switch(type) {
	case Carrier:
	case WarSun:
		return 6;
	case Dreadnought:
	case Cruiser:
		if(players[player].technologies.is(StasisCapsules))
			return 1;
		return 0;
	default:
		return 0;
	}
}

unit_s unit::getcapacitylimit() const {
	switch(type) {
	case Dreadnought:
	case Cruiser:
		if(players[player].technologies.is(StasisCapsules))
			return GroundForces;
		return NoUnit;
	default:
		return NoUnit;
	}
}

int	unit::getcarried() const {
	auto result = 0;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.parent == this)
			result++;
	}
	return result;
}

int unit::getjoincount(unit_s object) const {
	auto maximum = getcapacity();
	if(!maximum)
		return 0;
	auto limit = getcapacitylimit();
	if(limit && object != limit)
		return 0;
	auto current = getcarried();
	return maximum - current;
}

int	unit::getfightersupport() {
	auto result = 0;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.player != player)
			continue;
		if(!e.parent)
			continue;
		if(e.type == SpaceDock && e.parent->parent != this)
			continue;
		else if(e.parent != this)
			continue;
		auto capacity_count = e.getcapacity();
		if(!capacity_count)
			continue;
		auto capacity_limit = e.getcapacitylimit();
		if(capacity_limit != NoUnit && capacity_limit != Fighters)
			continue;
		result += capacity_count;
	}
	return result;
}

bool unit::build(unit_s object, bool run) {
	auto solar_system = get(SolarSystem);
	if(!solar_system)
		return false;
	auto produce_count = getproduction(object);
	auto available_count = getavailable(object);
	if(available_count) {
		auto exist_count = getcount(object, player);
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	auto build_base = get(Planet);
	if(!build_base)
		return false;
	switch(object) {
	case Fighters:
	case Carrier:
	case Cruiser:
	case Destroyer:
	case Dreadnought:
	case WarSun:
		build_base = solar_system;
		break;
	}
	if(build_base->player != player)
		return false;
	if(object==Fighters && !players[player].technologies.is(AdvancedFighters)) {
		auto available_count = solar_system->getfightersupport();
		auto exist_count = getcount(object, player, solar_system);
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	if(run) {
		for(auto i = 0; i < produce_count; i++)
			new unit(object, build_base, player);
	}
	return true;
}

bool unit::in(const unit* object) const {
	for(auto p = this; p; p = p->parent) {
		if(p == object)
			return true;
	}
	return false;
}

int unit::getweight() const {
	auto result = getresource()*2;
	auto build_count = getproduction(type);
	if(build_count)
		result = result / build_count;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.parent == this)
			result += e.getweight();
	}
	return result;
}

void unit::destroy() {
}