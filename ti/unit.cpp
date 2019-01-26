#include "main.h"

static struct unit_data_info {
	const char*	id;
	const char*	name;
	char		available;
	char		cost;
	char		production;
	char		movements;
	char		count;
	weapon_info	combat;
} unit_type_data[] = {{""},
{"Solar", "Звездная система", 0, 0, 0, 0},
{"Nebula", "Небула", 0, 0, 0, 0},
{"Planet", "Планета", 0, 0, 0, 0},
{"SpaceDock", "Доки", 3, 4, 1, 0},
{"GroundForces", "Наземные силы", 0, 1, 2, 0, 2, 8},
{"Fighters", "Истребители", 0, 1, 2, 0, 2, 9},
{"PDS", "СПЗ", 6, 2, 1, 0, 1, 6},
{"Carrier", "Транспорт", 4, 3, 1, 1, 1, 9},
{"Cruiser", "Крейсер", 8, 2, 1, 2, 1, 7},
{"Destroyer", "Эсминец", 8, 1, 1, 2, 1, 9},
{"Dreadnought", "Дредноут", 5, 5, 1, 1, 1, 5},
{"WarSun", "Звезда смерти", 2, 12, 1, 2, 1, {3, 3}},
};
getstr_enum(unit_type);
adat<unit_info, 256>	units;

unsigned select(unit_info** result, unit_info** result_max, const unit_info* location, const player_info* player, bool (unit_info::*test)() const) {
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
	auto result = unit_type_data[type].movements;
	switch(type) {
	case Carrier:
		if(player->is(XRDTransporters))
			result++;
		break;
	case Cruiser:
		if(player->is(TypeIVDrive))
			result++;
		break;
	case Dreadnought:
		if(player->is(TypeIVDrive))
			result++;
		break;
	case Fighters:
		if(player->is(AdvancedFighters))
			result = 2;
		break;
	}
	return result;
}

int	unit_info::getresource() const {
	auto result = unit_type_data[type].cost;
	switch(type) {
	case Dreadnought:
		if(player->is(BonusCostDreadnought))
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
	return unit_type_data[type].available;
}

int	unit_info::getproduction(unit_type_s type) {
	return unit_type_data[type].production;
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
	auto w = unit_type_data[type].combat;
	if(player) {
		if(player->is(CombatBonusAll))
			w.bonus++;
		if(player->is(CombatPenalty))
			w.bonus--;
		if(type == Dreadnought && player->is(CombatBonusDreadnought))
			w.bonus++;
		if(type == Fighters && player->is(CombatBonusFighters))
			w.bonus++;
		switch(type) {
		case Fighters:
			if(player->is(Cybernetics))
				w.bonus++;
			if(player->is(AdvancedFighters))
				w.bonus++;
			break;
		case GroundForces:
			if(player->is(GenSynthesis))
				w.bonus++;
			break;
		case Cruiser:
		case Destroyer:
			if(player->is(HylarVAssaultLaser))
				w.bonus++;
			break;
		case PDS:
			if(player->is(MagenDefenseGrid))
				w.bonus++;
			if(player->is(GravitonLaserSystem))
				w.reroll++;
			break;
		}
	}
	return w;
}

weapon_info unit_info::getweapon(bool attacker, const player_info* opponent, char round) const {
	auto w = getweapon();
	if(type == GroundForces && attacker && player->is(CombatBonusGroundForcesAttack))
		w.bonus++;
	if(attacker && round == 1 && opponent->is(CombatBonusDefend))
		w.bonus--;
	return w;
}

bool unit_info::isinvaders() const {
	switch(type) {
	case GroundForces:
		return true;
	case Fighters:
		return player->is(GravitonNegator);
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

int	unit_info::getcount() const {
	return unit_type_data[type].count;
}

int	unit_info::getcount(unit_type_s type, const player_info* player, unit_info* location) {
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
		if(player->is(StasisCapsules))
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
		if(player->is(StasisCapsules))
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
	if(object==Fighters && !player->is(AdvancedFighters)) {
		auto available_count = solar_system->getfightersupport();
		auto exist_count = getcount(object, player, solar_system);
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	if(run) {
		for(auto i = 0; i < produce_count; i++) {
			auto p = new unit_info(object);
			p->parent = build_base;
			p->player = player;
		}
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