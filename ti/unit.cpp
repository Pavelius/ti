  #include "main.h"

INSTDATAC(uniti, 32 * 6);

void* uniti::operator new(unsigned size) {
	for(auto& e : bsdata<uniti>()) {
		if(!e)
			return &e;
	}
	return bsdata<uniti>::add();
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

int uniti::getmaxhits() const {
	switch(type) {
	case Dreadnought:
	case WarSun:
		return 2;
	default:
		return 1;
	}
}

int	uniti::getcapacity() const {
	int i;
	switch(type) {
	case Carrier:
		i = 4;
		if(is(CarrierII))
			i += 2;
		return i;
	case WarSun:
		return 6;
	case Dreadnought:
		return 0;
	case Cruiser:
		if(is(CruiserII))
			return 1;
		return 0;
	case SpaceDock:
		return 3;
	default:
		return 0;
	}
}

weaponi uniti::getweapon() const {
	auto w = getgroup().combat;
	if(is(CombatBonusAll))
		w.bonus++;
	if(is(CombatPenalty))
		w.bonus--;
	switch(type) {
	case Cruiser:
		if(is(CruiserII))
			w.bonus++;
		break;
	case Fighters:
		if(is(FighterII))
			w.bonus++;
		break;
	case Destroyer:
		if(is(DestroyerII))
			w.bonus++;
		break;
	case GroundForces:
		if(is(InfantryII))
			w.bonus++;
		break;
	case PDS:
		if(is(PDSII))
			w.bonus++;
		break;
	}
	return w;
}

int uniti::getmovement() const {
	auto i = getgroup().movements;
	switch(type) {
	case Carrier:
		if(is(CarrierII))
			i++;
		break;
	case Cruiser:
		if(is(CruiserII))
			i++;
		break;
	case Dreadnought:
		if(is(DreadnoughtII))
			i++;
		break;
	case Fighters:
		if(is(FighterII))
			i = 2;
		break;
	}
	return i;
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
	//case Fighters:
	//	return is(GravitonNegator);
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

int	uniti::getcarried() const {
	auto result = 0;
	for(auto& e : bsdata<uniti>()) {
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
	auto current = getcarried();
	return maximum - current;
}

bool uniti::build(variant_s object, bool run) {
	auto player = getplayer();
	if(!player)
		return false;
	auto planet = getplanet();
	if(!planet)
		return false;
	auto solar = planet->getsolar();
	if(!solar)
		return false;
	if(uniti::isplanetary(object) && solar->getplayer() && solar->getplayer() != player)
		return false;
	auto available_count = bsdata<varianti>::elements[object].available;
	if(available_count) {
		auto exist_count = player->getcount(object);
		if(exist_count >= available_count)
			return false;
	}
	if(run) {
		auto p = new uniti(object);
		if(uniti::isplanetary(object))
			p->setplanet(planet);
		else
			p->setsolar(solar);
		p->setplayer(player);
	}
	return true;
}

int uniti::getweight() const {
	auto result = getresource() * 2;
	auto build_count = bsdata<varianti>::elements[type].production;
	if(build_count)
		result = result / build_count;
	return result;
}

void uniti::destroy() {}

playeri* uniti::getplayer() const {
	if(player == 0xFF)
		return 0;
	return &bsdata<playeri>::elements[player];
}

void uniti::setplayer(const playeri* v) {
	if(!v)
		player = 0xFF;
	else
		player = v - bsdata<playeri>::elements;
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