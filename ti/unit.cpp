#include "main.h"

groupi bsmeta<groupi>::elements[] = {{""},
{"Solar", "Звездная система", 0, 0, 0, 0},
{"AsteroidField", "Поле астероидов", 0, 0, 0, 0},
{"Nebula", "Небула", 0, 0, 0, 0},
{"Supernova", "Супернова", 0, 0, 0, 0},
{"Planet", "Планета", 0, 0, 0, 0},
{"SpaceDock", "Доки", 3, 4, 1, 0, 0, 2},
{"GroundForces", "Наземные силы", 0, 1, 2, 0, 2, 8},
{"Fighters", "Истребители", 0, 1, 2, 0, 2, 9},
{"PDS", "СПЗ", 6, 2, 1, 0, 1, 6},
{"Carrier", "Транспорт", 4, 3, 1, 1, 1, 9},
{"Cruiser", "Крейсер", 8, 2, 1, 2, 1, 7},
{"Destroyer", "Эсминец", 8, 1, 1, 2, 1, 9},
{"Dreadnought", "Линкор", 5, 5, 1, 1, 1, 5},
{"WarSun", "Звезда смерти", 2, 12, 1, 2, 1, 0, {3, 3}},
};
assert_enum(group, WarSun);
DECLBASE(uniti, 32 * 6);

unsigned select(uniti** result, uniti** result_max, const uniti* location, const playeri* player, bool (uniti::*test)() const) {
	auto p = result;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
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

uniti* getminimal(uniti** result, unsigned count, int (uniti::*get)() const) {
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

void* uniti::operator new(unsigned size) {
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			return &e;
	}
	return bsmeta<uniti>::add();
}

uniti::~uniti() {
	type = NoUnit;
}

const char* uniti::getname() const {
	if(issolar())
		return getsolarname();
	if(isplanet())
		return getplanetname();
	return bsmeta<groupi>::elements[type].name;
}

bool uniti::is(tech_s v) const {
	if(!player)
		return false;
	return player.getplayer()->is(v);
}

bool uniti::is(bonus_s v) const {
	if(!player)
		return false;
	return player.getplayer()->is(v);
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

int	uniti::getavailable(group_s type) {
	return bsmeta<groupi>::elements[type].available;
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

int	uniti::getproduce(group_s type) {
	return bsmeta<groupi>::elements[type].production_count;
}

int	uniti::getproduction(group_s type) {
	return bsmeta<groupi>::elements[type].production;
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

bool uniti::isplanetary(group_s type) {
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

int	uniti::getcount(group_s type, const playeri* player, uniti* location) {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.type == type && e.player == player)
			result++;
		if(location && !e.in(location))
			continue;
	}
	return result;
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

group_s uniti::getcapacitylimit() const {
	switch(type) {
	case Dreadnought:
	case Cruiser:
		if(is(StasisCapsules))
			return GroundForces;
		return NoUnit;
	default:
		return NoUnit;
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

int uniti::getjoincount(group_s object) const {
	auto maximum = getcapacity();
	if(!maximum)
		return 0;
	auto limit = getcapacitylimit();
	if(limit && object != limit)
		return 0;
	auto current = getcarried();
	return maximum - current;
}

int	uniti::getfightersupport() {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.player != player)
			continue;
		if(!e.parent)
			continue;
		if(e.parent.type == PlanetVar && e.parent.getplanet()->getsolar() != this)
			continue;
		if(e.parent.type == Solar && e.parent.getsolar() != this)
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

bool uniti::build(group_s object, bool run) {
	auto solar_system = getsolar();
	if(!solar_system)
		return false;
	auto produce_count = getproduction(object);
	auto available_count = getavailable(object);
	if(available_count) {
		auto exist_count = getcount(object, player.getplayer());
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	uniti* build_base = getplanet();
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
	if(object == Fighters && !is(AdvancedFighters)) {
		auto available_count = solar_system->getfightersupport();
		auto exist_count = getcount(object, player.getplayer(), solar_system);
		if(exist_count + produce_count > available_count)
			produce_count = available_count - exist_count;
	}
	if(produce_count <= 0)
		return false;
	if(run) {
		for(auto i = 0; i < produce_count; i++) {
			auto p = new uniti(object);
			p->parent = build_base;
			p->player = player;
		}
	}
	return true;
}

bool uniti::in(const uniti* object) const {
	auto planet = getplanet();
	if((uniti*)planet == object)
		return true;
	auto solar = getsolar();
	if((uniti*)solar == object)
		return true;
	return false;
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

void uniti::destroy() {
}

int	uniti::getfleet(const playeri* player, const uniti* solar) {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getsolar() != solar)
			continue;
		if(!e.isfleet())
			continue;
		result++;
	}
	return result;
}

int	uniti::getfleet(const playeri* player) {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(!e.isfleet())
			continue;
		result++;
	}
	return result;
}

//unsigned uniti::select(uniti** result, uniti* const* result_max, uniti* parent) {
//	auto p = result;
//	for(auto& e : bsmeta<uniti>()) {
//		if(e.parent != parent)
//			continue;
//		if(p < result_max)
//			*p++ = &e;
//	}
//	return p - result;
//}

solari* uniti::getsolar() const {
	if(parent.type == Solar)
		return parent.getsolar();
	return 0;
}

void uniti::update_control() {
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		e.setplayer(0);
		for(auto& u : bsmeta<uniti>()) {
			if(!u)
				continue;
			if(u.getsolar() == &e) {
				u.setplayer(e.getplayer());
				break;
			}
		}
	}
}

bool uniti::isunit() const {
	return this >= bsmeta<uniti>::elements
		&& this < (bsmeta<uniti>::elements + bsmeta<uniti>::count);
}

uniti* uniti::get(target_s v) const {
	if(!this)
		return 0;
	switch(v) {
	case TargetSystem:
		if(issolar())
			return const_cast<uniti*>(this);
		return (uniti*)getsolar();
	case TargetPlanet:
		if(isplanet())
			return const_cast<uniti*>(this);
		return (uniti*)getplanet();
	default:
		if(issolar() || isplanet())
			return 0;
		return const_cast<uniti*>(this);
	}
}

//uniti* uniti::find(group_s v, const playeri* player) const {
//	for(auto& e : bsmeta<uniti>()) {
//		if(!e)
//			continue;
//		if(e.parent != this)
//			continue;
//		if(player && e.player != player)
//			continue;
//		return &e;
//	}
//	return 0;
//}

bool uniti::isactivated(const playeri* player) const {
	if(!player)
		return true;
	return (activate_flags & (1 << (player->getid()))) != 0;
}

void uniti::activate() {
	activate_flags = 0xFF;
}

void uniti::deactivate() {
	activate_flags = 0;
}

void uniti::activate(const playeri* player, bool setvalue) {
	if(!player)
		return;
	variant v = player;
	if(setvalue)
		activate_flags |= 1 << v.value;
	else
		activate_flags &= ~(1 << v.value);
	player->slide(this);
}

playeri* uniti::getplayer() const {
	if(player.type == Player)
		return player.getplayer();
	return 0;
}

void uniti::setplayer(const playeri* v) {
	player = const_cast<playeri*>(v);
}

void uniti::setplanet(const planeti* v) {
	parent = const_cast<planeti*>(v);
}

void uniti::setsolar(const solari* v) {
	parent = const_cast<solari*>(v);
}