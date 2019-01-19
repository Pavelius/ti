#include "main.h"

static struct unit_data_info {
	const char*	id;
	const char*	name;
	char		available;
	char		cost;
	char		production;
	char		movements;
	weapon_info		combat;
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
getstr_enum(unit_info);
adat<unit_info, 256>	units;

unsigned select(unit_info** result, unit_info** result_max, unit_info* location, player_s player, bool (unit_info::*test)() const) {
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

unit_info* getminimal(unit_info** result, unsigned count, int (unit_info::*get)() const) {
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

void* unit_info::operator new(unsigned size) {
	for(auto& e : units) {
		if(!e)
			return &e;
	}
	return units.add();
}

unit_info::~unit_info() {
	type = NoUnit;
}

const char* unit_info::getname() const {
	return getstr(type);
}

int unit_info::getmovement() const {
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

int	unit_info::getresource() const {
	auto result = unit_data[type].cost;
	switch(player) {
	case TheL1z1xMindnet:
		if(type == Dreadnought)
			result--;
		break;
	}
	return result;
}

unit_info* unit_info::get(unit_type_s parent_type) {
	auto result = this;
	while(result && result->type != parent_type)
		result = result->parent;
	return result;
}

int	unit_info::getavailable(unit_type_s type) {
	return unit_data[type].available;
}

int	unit_info::getproduction(unit_type_s type) {
	return unit_data[type].production;
}

int unit_info::getmaxhits() const {
	switch(type) {
	case Dreadnought:
	case WarSun:
		return 2;
	default:
		return 1;
	}
}

weapon_info unit_info::getweapon() const {
	auto w = unit_data[type].combat;
	if(is(SardakkNOrr))
		w.bonus++;
	if(is(TheL1z1xMindnet)) {
		if(type == Dreadnought)
			w.bonus++;
	}
	if(is(TheNaaluCollective)) {
		if(type == Fighters)
			w.bonus++;
	}
	if(is(UniversitiesOfJolNar))
		w.bonus--;
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

weapon_info unit_info::getweapon(bool attacker, player_s opponent, char round) const {
	auto w = getweapon();
	if(is(TheL1z1xMindnet)) {
		if(type == GroundForces && attacker)
			w.bonus++;
	}
	if(players[opponent].is(TheXxchaKingdom) && round == 1)
		w.bonus--;
	return w;
}

bool unit_info::isinvaders() const {
	switch(type) {
	case GroundForces:
		return true;
	case Fighters:
		return players[player].technologies.is(GravitonNegator);
	default:
		return false;
	}
}

bool unit_info::isplanetary(unit_type_s type) {
	switch(type) {
	case GroundForces:
	case PDS:
		return true;
	default:
		return false;
	}
}

int	unit_info::getcount(unit_type_s type, player_s player, unit_info* location) {
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

int	unit_info::getcapacity() const {
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

unit_type_s unit_info::getcapacitylimit() const {
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

int	unit_info::getcarried() const {
	auto result = 0;
	for(auto& e : units) {
		if(!e)
			continue;
		if(e.parent == this)
			result++;
	}
	return result;
}

int unit_info::getjoincount(unit_type_s object) const {
	auto maximum = getcapacity();
	if(!maximum)
		return 0;
	auto limit = getcapacitylimit();
	if(limit && object != limit)
		return 0;
	auto current = getcarried();
	return maximum - current;
}

int	unit_info::getfightersupport() {
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

bool unit_info::build(unit_type_s object, bool run) {
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
			new unit_info(object, build_base, player);
	}
	return true;
}

bool unit_info::in(const unit_info* object) const {
	for(auto p = this; p; p = p->parent) {
		if(p == object)
			return true;
	}
	return false;
}

int unit_info::getweight() const {
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

void unit_info::destroy() {
}