#include "main.h"

bsreq player_type[] = {
	BSREQ(player_info, id, text_type),
	BSREQ(player_info, name, text_type),
{}};

struct player_pregen_info {
	const char*			id;
	const char*			name;
	char				tokens[3];
	cflags<bonus_s>		bonus;
	adat<action_s, 8>	actions;
	unit_type_s			start_units[16];
	cflags<tech_s>		start_tech;
};
static player_pregen_info player_pregen_data[] = {{"xxcha", "Королевство Иксча", {2, 3, 3},
{CombatBonusDefend},
{ExecutePrimaryAbility, ChangePoliticCard},
{Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser},
{AntimassDeflectors, EnviroCompensator}},
{"barony", "Баронство Летнева", {2, 3, 3},
{BonusFleetTokens},
{BaronyEquipment},
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{HylarVAssaultLaser, AntimassDeflectors}
},
{"naalu", "Община Наалу", {2, 3, 3},
{BonusInitiative, CombatBonusFighters},
{NaaluFleetRetreat},
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{EnviroCompensator, AntimassDeflectors}
},
{"mindnet", "Сеть раума L1z1x", {3, 3, 3},
{BonusCostDreadnought, BonusCostDreadnought, CombatBonusGroundForcesAttack},
{},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{EnviroCompensator, StasisCapsules, Cybernetics, HylarVAssaultLaser}
},
{"yssaril", "Племена Изарилов", {2, 3, 3},
{BonusActionCards},
{LookActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{Cybernetics, AntimassDeflectors}
},
{"sol", "Федерация Солнца", {2, 3, 3},
{BonusCommandCounter},
{SolOrbitalDrop},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{AntimassDeflectors, XRDTransporters}
},
{"mentax", "Коалиция Ментаков", {2, 3, 4},
{},
{MentakAmbush, MentakPiracy},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{EnviroCompensator, HylarVAssaultLaser}
},
{"hacan", "Эмираты Хакканов", {2, 3, 3},
{BonusTrade},
{HacanTradeActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{EnviroCompensator, SarweenTools}
},
{"jelnar", "Университеты Джолнаров", {2, 3, 3},
{CombatPenalty, BonusTechnology},
{JolanrRerollCombatDices},
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, HylarVAssaultLaser, EnviroCompensator, SarweenTools}
},
{"norr", "Сардак Норры", {2, 3, 3},
{CombatBonusAll},
{},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, PDS},
{DeepSpaceCannon, HylarVAssaultLaser}
},
};

player_info				players[6];
static player_info*		speaker;
static player_info*		human_player;
static player_info*		diplomacy_players[2];

static int compare_planets(const void* p1, const void* p2) {
	auto e1 = *((planet_info**)p1);
	auto e2 = *((planet_info**)p2);
	return e2->resource - e1->resource;
}

player_info* player_info::find(const char* id) {
	for(auto& e : players) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

bool player_info::iscomputer() const {
	return this != human_player;
}

bool player_info::isally(player_info* enemy) const {
	return (this == enemy)
		|| (diplomacy_players[0] == this && diplomacy_players[1] == enemy)
		|| (diplomacy_players[0] == enemy && diplomacy_players[1] == this);
}

bool player_info::isallow(unit_type_s id) const {
	switch(id) {
	case WarSun:
		return is(WarSunTech);
	default:
		return true;
	}
}

void player_info::sethuman() {
	human_player = this;
}

unit_info* player_info::create(unit_type_s id, unit_info* planet) {
	auto p = new unit_info(id);
	p->player = this;
	p->parent = planet;
	return p;
}

const player_pregen_info* find_by_id(const char* id) {
	for(auto& e : player_pregen_data) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static void create_start_units(player_info* player) {
	auto p = find_by_id(player->id);
	assert(p);
	adat<planet_info*, 8> planets;
	planets.count = planet_info::select(planets.data, planets.endof(), p->id);
	assert(planets.count);
	qsort(planets.data, planets.count, sizeof(planets.data[0]), compare_planets);
	auto base_planet = planets.data[0];
	auto solar_system = planets.data[0]->parent;
	player->create(SpaceDock, base_planet);
	for(auto e : p->start_units) {
		if(!e)
			break;
		auto base = solar_system;
		switch(e) {
		case GroundForces:
		case PDS:
			base = base_planet;
			break;
		}
		player->create(e, base);
	}
}

void player_info::create(const char* id) {
	memset(this, 0, sizeof(*this));
	auto p = find_by_id(id);
	assert(p);
	this->id = p->id;
	this->name = p->name;
	technologies = p->start_tech;
	bonuses = p->bonus;
	// Game setup: step 11
	set(Strategy, p->tokens[0]);
	set(Command, p->tokens[1]);
	set(Fleet, p->tokens[2]);
}

const char* player_info::getid() const {
	return id;
}

int	player_info::getindex() const {
	if(!this)
		return 0;
	return this - players;
}

const char* player_info::getname() const {
	return name;
}

const char* player_info::getyouname() const {
	if(this == gethuman())
		return "Вы";
	return getname();
}

int	player_info::getinitiative() const {
	auto result = getinitiative(strategy);
	if(is(BonusInitiative))
		result = 0;
	return result;
}

player_info* player_info::getspeaker() {
	return speaker;
}

player_info* player_info::gethuman() {
	return human_player;
}

int	player_info::get(action_s id) const {
	int r = cost_info::get(id);
	switch(id) {
	case Fleet:
		if(is(BonusFleetTokens))
			r++;
		break;
	}
	return r;
}

void player_info::getinfo(string& sb) const {
	sb.add("###%1", getname());
	if(strategy)
		sb.addn("[+%1 стратегия]", getstr(strategy));
	sb.addn("%1i ресурсов", planet_info::get(this, &planet_info::getresource));
	sb.addn("%1i влияния", planet_info::get(this, &planet_info::getinfluence));
	sb.addn("%1i стратегических маркеров", get(Strategy));
	sb.addn("%1i тактических маркеров", get(Command));
	sb.addn("%1i маркеров флота", get(Fleet));
	sb.addn("%1i товаров", get(Goods));
	if(getspeaker() == this)
		sb.addn("Являются [спикером] сената.");
	if(gethuman() == this)
		sb.addn("Это [ваша] нация.");
}

void player_info::setup() {
	create_action_deck();
	speaker = &players[rand() % (sizeof(players) / sizeof(players[0]))];
	planet_info::setup();
	for(auto& e : players)
		create_start_units(&e);
	unit_info::update_control();
}

static void select(player_array& source, const player_info* start) {
	auto index = start - players;
	for(unsigned i = 0; i < lenghtof(players); i++) {
		source.add(&players[index++]);
		if(index >= (int)lenghtof(players))
			index = 0;
	}
}

static void strategic_phase() {
	player_array source;
	select(source, player_info::getspeaker());
	for(auto p : source)
		p->strategy = NoStrategy;
	answer_info ai;
	adat<strategy_s, Imperial + 1> politics;
	for(auto i = Initiative; i <= Imperial; i = (strategy_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		ai.clear();
		if(!p->iscomputer()) {
			for(auto e : politics)
				ai.add(e, getstr(e));
			ai.sort();
			p->strategy = (strategy_s)ai.choose(
				"Эта стратегическая фаза. Вам нужно выбрать одну стратегию из списка ниже, которую будете использовать на этот ход. Будьте внимательны. Ваши враги также выбирают одну стратегию из этого же списка.", p);
		} else {
			p->strategy = politics.data[rand() % politics.getcount()];
			string sb;
			sb.add("Наши враги [%1] определились со своим курсом действий на ближайшее время. По их решению это стала [%2] стратегия. Будьте внимательны и осторожны.", p->getname(), getstr(p->strategy));
			player_info::report(sb);
		}
		politics.remove(politics.indexof(p->strategy));
	}
}

int player_info::getcardscount() const {
	auto result = 0;
	for(auto i = FirstActionCard; i <= LastActionCard; i = (action_s)(i + 1))
		result += get(i);
	return result;
}

void player_info::check_card_limin() {

}

action_s player_info::report(const string& sb) {
	answer_info ai;
	ai.add(0, "Принять");
	return (action_s)ai.choosev(false, 0, gethuman()->getid(), sb);
}

player_info* player_info::choose_opponent(const char* text) {
	answer_info ai;
	for(auto& e : players) {
		if(this == &e)
			continue;
		ai.add((int)&e, e.getname());
	}
	return (player_info*)ai.choose(text, this);
}

void player_info::add_action_cards(int value) {
	for(auto i = 0; i < value; i++) {
		auto a = action_deck.draw();
		add(a, 1);
	}
	string sb;
	sb.add("%1 получили %2i получили карт действий.", getyouname(), value);
	report(sb);
}

void player_info::add_command_tokens(int value) {
	static action_s command_area[] = {Strategy, Command, Fleet};
	while(value > 0) {
		string sb;
		answer_info ai;
		sb.add("%1 получили %2i командных очков.", getyouname(), value);
		sb.adds("Распределите [%1i] очко.", value);
		ai.clear();
		for(auto e : command_area)
			ai.add(e, "Очки %1", getstr(e));
		auto a = (action_s)ai.choose(sb, this);
		add(a, 1);
		value--;
	}
}

int	player_info::getfleet() const {
	return get(Fleet);
}

unit_info* player_info::gethomesystem() const {
	auto index = getindex();
	return solars + 33 + index;
}

void player_info::build_units(int value) {
	army result;
	select(result, TargetPlanet | Friendly | DockPresent);
	auto planet = static_cast<planet_info*>(choose(result, "Укажите планету, на которой будете строить"));
	if(!planet)
		return;
	auto solar = planet->get(TargetSystem);
	auto dock = planet->find(SpaceDock, this);
	auto dock_produce = 0;
	if(dock)
		dock_produce = dock->getproduce();
	if(iscomputer()) {

	} else {
		if(build(result, planet, solar, getresource(), getfleet(), 0, dock_produce, true))
			unit_info::update_control();
	}
}

int	player_info::getresource() const {
	return planet_info::get(this, &planet_info::getresource);
}

unit_info* player_info::choose(army& source, const char* format) const {
	answer_info ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	if(!ai)
		return 0;
	return (unit_info*)ai.choose(format, this);
}

static void refresh_players() {
	memset(diplomacy_players, 0, sizeof(diplomacy_players));
	for(auto& e : players) {
		if(e.strategy == Initiative) {
			e.set(StrategyAction, 0);
			speaker = &e;
		} else
			e.set(StrategyAction, 1);
		e.set(TacticalAction, 1);
		e.set(TransferAction, 1);
		e.set(Pass, 1);
	}
}

unit_info* player_info::choose_solar() const {
	return 0;
}

void player_info::add_peace_pact(int value) {
	string sb;
	sb.player = this;
	sb.add("Выбирайте оппонента с которым вы будете в сознических отношениях до конца этого хода. Ни он не вы не сможете нападать друг на друга.");
	diplomacy_players[0] = this;
	diplomacy_players[1] = choose_opponent(sb);
	if(iscomputer()) {
		sb.add("%1 выбрали %2.", getname(), diplomacy_players[1]->getname());
		report(sb);
	}
}

static void strategy_primary_action(player_info* p, strategy_s id) {
	switch(id) {
	case Diplomacy:
		p->add_peace_pact(1);
		break;
	case Political:
		p->add_action_cards(3);
		p->add_command_tokens(1);
		p->draw_political_card(1);
		p->predict_next_political_card(3);
		break;
	case Logistics:
		p->add_command_tokens(4);
		break;
	case Trade:
		if(p->choose_trade()) {
			p->add_trade_goods(3);
			p->add_profit_for_trade_agreements();
			p->open_trade_negatiation();
		} else
			p->cancel_all_trade_agreements();
		break;
	case Warfare:
		p->return_command_from_board(1);
		break;
	case Technology:
		p->add_technology(1);
		break;
	case Imperial:
		p->add_objective(1);
		p->add_victory_points(2);
		break;
	}
}

static void strategy_secondanary_action(player_info* p, strategy_s id) {
	switch(id) {
	case Diplomacy:
		p->refresh_planets(1);
		break;
	case Political:
		p->add_action_cards(1);
		break;
	case Logistics:
		p->add_command_tokens(1);
		break;
	case Trade:
		p->add_profit_for_trade_agreements();
		break;
	case Warfare:
		break;
	case Technology:
		break;
	case Imperial:
		p->build_units(1);
		break;
	}
}

static void tactical_action(player_info* player) {
	auto solar = player->choose_solar();
	solar->activate(player);
	player->moveships(solar);
}

static action_s choose_action(player_info* p) {
	string sb;
	answer_info ai;
	sb.add("Что вы предпочитаете делать в свой ход?");
	for(auto a = Armistice; a <= LastAction; a = (action_s)(a + 1)) {
		if(!p->is(a) || !p->isallow(AsAction, a))
			continue;
		ai.add(a, getstr(a), getstr(p->strategy));
	}
	return (action_s)ai.choose(sb, p);
}

static void play_action(player_info* p, action_s id) {
	switch(id) {
	case StrategyAction:
		strategy_primary_action(p, p->strategy);
		break;
	case TacticalAction:
		break;
	}
}

static void action_phase() {
	const int last_initiative = 8;
	refresh_players();
	auto someone_move = true;
	while(someone_move) {
		someone_move = false;
		for(auto i = 0; i <= last_initiative; i++) {
			for(auto& e : players) {
				if(e.get(Pass) == 0)
					continue;
				if(e.getinitiative() != i)
					continue;
				auto a = choose_action(&e);
				play_action(&e, a);
				e.add(a, -1);
				someone_move = true;
			}
		}
	}
}

void player_info::make_move() {
	strategic_phase();
	action_phase();
}

int get_solar_map_index(int index) {
	for(unsigned i = 0; i < sizeof(solar_map) / sizeof(solar_map[0]); i++) {
		if(solar_map[i] == index)
			return i;
	}
	return -1;
}

void player_info::slide(const unit_info* p) {
	if(!p->issolar())
		return;
	auto index = get_solar_map_index(p - solars);
	slide(index);
}

unsigned player_info::select(unit_info** result, unit_info* const* pe, unsigned flags, unit_type_s type) const {
	auto ps = result;
	for(auto& e : units) {
		if(!e)
			continue;
		if(flags&Friendly && e.player != this)
			continue;
		if(e.type != SpaceDock)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - result;
}

void player_info::select(army& result, unsigned flags) const {
	if(flags&DockPresent) {
		result.count = select(result.begin(), result.endof(), flags, SpaceDock);
		result.transform((target_s)(flags&TargetMask));
		result.rollup();
		return;
	}
	switch(flags&TargetMask) {
	case TargetSystem:
		for(auto& e : solars) {
			if(!e)
				continue;
			if(flags&Friendly && e.player != this)
				continue;
			result.add(&e);
		}
		break;
	case TargetPlanet:
		for(auto& e : solars) {
			if(!e)
				continue;
			if(flags&Friendly && e.player != this)
				continue;
			result.add(&e);
		}
		break;
	}
}

void player_info::moveships(unit_info* solar) {

}