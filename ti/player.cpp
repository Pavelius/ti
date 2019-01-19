#include "main.h"

struct player_info_pregen {
	const char*			id;
	const char*			name;
	unit_s				start_units[16];
	cflags<tech_s>		start_tech;
};
static constexpr player_info_pregen player_data[] = {{""},
{"xxcha", "����������� �����",
{Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser},
{AntimassDeflectors, EnviroCompensator}
},
{"barony", "��������� �������",
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{HylarVAssaultLaser, AntimassDeflectors}
},
{"naalu", "������ �����",
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{EnviroCompensator, AntimassDeflectors}
},
{"mindnet", "���� ����� L1z1x",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{EnviroCompensator, StasisCapsules, Cybernetics, HylarVAssaultLaser}
},
{"yssaril", "������� ��������",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{Cybernetics, AntimassDeflectors}
},
{"sol", "��������� ������",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{AntimassDeflectors, XRDTransporters}
},
{"mentax", "�������� ��������",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{EnviroCompensator, HylarVAssaultLaser}
},
{"hacan", "������� ��������",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{EnviroCompensator, SarweenTools}
},
{"jelnar", "������������ ���������",
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, HylarVAssaultLaser, EnviroCompensator, SarweenTools}
},
{"norr", "������ �����",
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

unit* player_info::create(unit_s id, unit* planet) {
	return new unit(id, planet, getindex());
}

void player_info::initialize() {
	memset(this, 0, sizeof(*this));
	auto player = getindex();
	auto& pi = player_data[player];
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
	actions[Fleet] = 3;
	actions[Command] = 3;
	actions[Strategy] = 2;
	actions[Goods] = 0;
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
		return "��";
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

void player_info::add(action_s id, int v) {
	actions[id] += v;
	if(actions[id] < 0)
		actions[id] = 0;
}

int	player_info::get(action_s id) const {
	int r = actions[id];
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
		sb.addn("[+%1 ���������]", getstr(strategy));
	sb.addn("%1i ��������", planet_info::get(getindex(), &planet_info::getresource));
	sb.addn("%1i �������", planet_info::get(getindex(), &planet_info::getinfluence));
	sb.addn("%1i �������������� ��������", get(Strategy));
	sb.addn("%1i ����������� ��������", get(Command));
	sb.addn("%1i �������� �����", get(Fleet));
	sb.addn("%1i �������", get(Goods));
	if(getspeaker() == this)
		sb.addn("�������� [��������] ������.");
	if(gethuman() == this)
		sb.addn("��� [����] �����.");
}

void player_info::setup() {
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
	auto player = player_info::gethuman();
	adat<strategy_s, Imperial + 1> politics;
	for(auto i = Initiative; i <= Imperial; i = (strategy_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		if(!p->iscomputer()) {
			answer_info ai;
			for(auto e : politics)
				ai.add(e, getstr(e));
			ai.sort();
			p->strategy = (strategy_s)ai.choose("��� �������������� ����. ��� ����� ������� *����* ��������� �� ������ ����, ������� ������ ������������ �� ���� ���. ������ �����������. ���� ����� ����� �������� ���� ��������� �� ����� �� ������.");
		} else {
			p->strategy = politics.data[rand() % politics.getcount()];
			string sb;
			sb.add("���� ����� [%1] ������������ �� ����� ������ �������� �� ��������� �����. �� �� ������� ��� ����� [%2] ���������. ������ ����������� � ���������.", p->getname(), getstr(p->strategy));
			player->report(sb);
		}
		politics.remove(politics.indexof(p->strategy));
	}
}

action_s player_info::report(const string& sb) {
	answer_info ai;
	ai.add(0, "�������");
	return (action_s)ai.choosev(false, 0, gethuman()->getid(), sb);
}

player_info* player_info::choose_opponent(string& sb) {
	answer_info ai;
	for(auto p : active_players) {
		if(this == p)
			continue;
		ai.add((int)p, p->getname());
	}
	return (player_info*)ai.choose(sb, iscomputer());
}

void player_info::add_action_cards(int value) {
	string sb;
	sb.add("%1 �������� %2i ��������� �����.", getyouname(), value);
	report(sb);
}

void player_info::add_command_tokens(int value) {
	static action_s command_area[] = {Strategy, Command, Fleet};
	string sb;
	answer_info ai;
	sb.add("%1 �������� %2i ��������� �����.", getyouname(), value);
	auto p = sb.get();
	while(value > 0) {
		sb.set(p);
		sb.adds("������������ [%1i] ����.");
		ai.clear();
		for(auto e : command_area)
			ai.add(e, "���� %1", getstr(e));
		auto a = (action_s)ai.choose(sb);
		add(a, 1);
		value--;
	}
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

static void strategy_primary_action(player_info* p, strategy_s id) {
	string sb;
	player_array source; select(source, p->getspeaker());
	switch(id) {
	case Diplomacy:
		sb.add("��������������� ��������� �������������, ��� �� ���������� ������������ ��������������� � ����� ������. �� ��, �� ��, �� ������� �������� ���� �� �����.");
		if(!p->iscomputer())
			sb.adds("��� ������ ����� ��������� �� ���� ���?");
		diplomacy_players[0] = p;
		diplomacy_players[1] = p->choose_opponent(sb);
		if(p->iscomputer()) {
			sb.add("%1 ������� %2.", p->getname(), diplomacy_players[1]->getname());
			p->report(sb);
		}
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

static action_s choose_action(player_info* p) {
	if(p->iscomputer()) {
		return Pass;
	} else {
		string sb;
		answer_info ai;
		sb.add("��� �� ������������� ������ � ���� ���?");
		for(auto a = StrategyAction; a <= TransferAction; a = (action_s)(a + 1)) {
			if(!p->is(a))
				continue;
			ai.add(a, getstr(a), getstr(p->strategy));
		}
		for(auto a = Armistice; a <= WarFooting; a = (action_s)(a + 1)) {
			if(!p->is(a))
				continue;
			ai.add(a, getstr(a));
		}
		if(p->is(Pass) && p->get(StrategyAction)==0)
			ai.add(Pass, getstr(Pass));
		return (action_s)ai.choose(sb);
	}
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