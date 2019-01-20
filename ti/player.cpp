#include "main.h"

bsreq player_type[] = {
	BSREQ(player_info, id, text_type),
	BSREQ(player_info, name, text_type),
{}};

struct player_info_pregen {
	const char*			id;
	const char*			name;
	unit_type_s				start_units[16];
	cflags<tech_s>		start_tech;
};
static constexpr player_info_pregen player_data[] = {{""},
{"xxcha", "Королевство Иксча",
{Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser},
{AntimassDeflectors, EnviroCompensator}
},
{"barony", "Баронство Летнева",
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{HylarVAssaultLaser, AntimassDeflectors}
},
{"naalu", "Община Наалу",
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{EnviroCompensator, AntimassDeflectors}
},
{"mindnet", "Сеть раума L1z1x",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{EnviroCompensator, StasisCapsules, Cybernetics, HylarVAssaultLaser}
},
{"yssaril", "Племена Изарилов",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{Cybernetics, AntimassDeflectors}
},
{"sol", "Федерация Солнца",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{AntimassDeflectors, XRDTransporters}
},
{"mentax", "Коалиция Ментаков",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{EnviroCompensator, HylarVAssaultLaser}
},
{"hacan", "Эмираты Хакканов",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{EnviroCompensator, SarweenTools}
},
{"jelnar", "Университеты Джолнаров",
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, HylarVAssaultLaser, EnviroCompensator, SarweenTools}
},
{"norr", "Сардак Норры",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, PDS},
{DeepSpaceCannon, HylarVAssaultLaser}
},
};
getstr_enum(player);

player_array			active_players;
player_info				players[SardakkNOrr + 1];
static player_info*		speaker;
static player_info*		human_player;
static player_info*		diplomacy_players[2];

static int compare_planets(const void* p1, const void* p2) {
	auto e1 = *((planet_info**)p1);
	auto e2 = *((planet_info**)p2);
	return e2->resource - e1->resource;
}

bool player_info::is(player_s value) const {
	return (this - players) == value;
}

bool player_info::iscomputer() const {
	return this != human_player;
}

bool player_info::isally(player_info* enemy) const {
	return (this == enemy)
		|| (diplomacy_players[0] == this && diplomacy_players[1] == enemy)
		|| (diplomacy_players[0] == enemy && diplomacy_players[1] == this);
}

void player_info::sethuman(player_s id) {
	human_player = players + id;
}

unit_info* player_info::create(unit_type_s id, unit_info* planet) {
	return new unit_info(id, planet, getindex());
}

void player_info::initialize() {
	memset(this, 0, sizeof(*this));
	auto player = getindex();
	auto& pi = player_data[player];
	id = pi.id;
	name = pi.name;
	technologies.data = pi.start_tech.data;
	// Game setup: step 10
	auto solar_system = solars + player;
	adat<planet_info*, 8> planets;
	//planets.count = select(planets.data, planets.data + planets.getmaximum(), solar_system);
	//if(!planets.count)
	//	return;
	qsort(planets.data, planets.count, sizeof(planets.data[0]), compare_planets);
	auto base_planet = planets.data[0];
	create(SpaceDock, base_planet);
	for(auto e : pi.start_units) {
		if(!e)
			break;
		auto base = solar_system;
		switch(e) {
		case GroundForces:
		case PDS:
			base = base_planet;
			break;
		}
		create(e, base);
	}
	// Game setup: step 11
	cost_info::initialize();
}

player_s player_info::getindex() const {
	return (player_s)(this - players);
}

const char* player_info::getid() const {
	return player_data[getindex()].id;
}

const char* player_info::getname() const {
	return player_data[getindex()].name;
}

const char* player_info::getyouname() const {
	if(this == gethuman())
		return "Вы";
	return getname();
}

int	player_info::getinitiative() const {
	auto result = getinitiative(strategy);
	if(is(TheNaaluCollective))
		result = 0;
	return result;
}

void player_info::add(player_s id) {
	active_players.add(players + id);
	players[id].initialize();
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
		if(is(TheBaronyOfLetnev))
			r++;
		break;
	}
	return r;
}

void player_info::getinfo(string& sb) const {
	sb.add("###%1", getname());
	if(strategy)
		sb.addn("[+%1 стратегия]", getstr(strategy));
	sb.addn("%1i ресурсов", planet_info::get(getindex(), &planet_info::getresource));
	sb.addn("%1i влияния", planet_info::get(getindex(), &planet_info::getinfluence));
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
	if(!active_players.count)
		return;
	speaker = active_players.data[rand()% active_players.count];
}

static void select(player_array& source, const player_info* start) {
	auto index = active_players.indexof((player_info*)start);
	if(index==-1)
		return;
	for(unsigned i = 0; i < active_players.count; i++) {
		source.add(active_players.data[index++]);
		if(index >= (int)active_players.count)
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
	for(auto p : active_players) {
		if(this == p)
			continue;
		ai.add((int)p, p->getname());
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

void player_info::build_units(int value) {

}

static void refresh_players() {
	memset(diplomacy_players, 0, sizeof(diplomacy_players));
	for(auto p : active_players) {
		if(p->strategy == Initiative) {
			p->set(StrategyAction, 0);
			speaker = p;
		} else
			p->set(StrategyAction, 1);
		p->set(TacticalAction, 1);
		p->set(TransferAction, 1);
		p->set(Pass, 1);
	}
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
	}
}

static void action_phase() {
	const int last_initiative = 8;
	refresh_players();
	auto someone_move = true;
	while(someone_move) {
		someone_move = false;
		for(auto i = 0; i <= last_initiative; i++) {
			for(auto p : active_players) {
				if(p->get(Pass) == 0)
					continue;
				if(p->getinitiative() != i)
					continue;
				auto a = choose_action(p);
				play_action(p, a);
				p->add(a, -1);
				someone_move = true;
			}
		}
	}
}

void player_info::make_move() {
	strategic_phase();
	action_phase();
}