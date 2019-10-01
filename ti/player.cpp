#include "main.h"

DECLBASE(playeri, 6);
struct player_pregen_info {
	const char*			id;
	const char*			name;
	char				tokens[3];
	cflags<bonus_s>		bonus;
	adat<action_s, 8>	actions;
	group_s			start_units[16];
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
static playeri*		active_player;
static playeri*		speaker;
static playeri*		human_player;

static int compare_planets(const void* p1, const void* p2) {
	auto e1 = *((planeti**)p1);
	auto e2 = *((planeti**)p2);
	return e2->resource - e1->resource;
}

void playeri::activate() {
	if(active_player != this) {
		active_player = this;
		auto solar = active_player->gethomesystem();
		if(solar)
			slide(solar->getindex());
	}
}

playeri* playeri::getactive() {
	return active_player;
}

playeri* playeri::find(const char* id) {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

bool playeri::iscomputer() const {
	return this != human_player;
}

bool playeri::isally(const playeri* enemy) const {
	return false;
}

bool playeri::isallow(group_s id) const {
	switch(id) {
	case WarSun:
		return is(WarSunTech);
	default:
		return true;
	}
}

void playeri::sethuman() {
	human_player = this;
}

uniti* playeri::create(group_s id, solari* solar) {
	auto p = new uniti(id);
	p->setplayer(this);
	p->setsolar(solar);
	return p;
}

uniti* playeri::create(group_s id, planeti* planet) {
	auto p = new uniti(id);
	p->setplayer(this);
	p->setplanet(planet);
	return p;
}

const player_pregen_info* find_by_id(const char* id) {
	for(auto& e : player_pregen_data) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static void create_start_units(playeri* player) {
	auto p = find_by_id(player->id);
	assert(p);
	adat<planeti*, 8> planets;
	planets.count = planeti::select(planets.data, planets.endof(), p->id);
	assert(planets.count);
	qsort(planets.data, planets.count, sizeof(planets.data[0]), compare_planets);
	auto base_planet = planets.data[0];
	auto solar_system = planets.data[0]->getsolar();
	player->create(SpaceDock, base_planet);
	for(auto e : p->start_units) {
		if(!e)
			break;
		switch(e) {
		case GroundForces:
		case PDS:
			player->create(e, base_planet);
			break;
		default:
			player->create(e, solar_system);
			break;
		}
	}
}

playeri& playeri::create(const char* id) {
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
	return *this;
}

int	playeri::getid() const {
	if(!this)
		return 0;
	return this - bsmeta<playeri>::elements;
}

const char* playeri::getname() const {
	return name;
}

const char* playeri::getyouname() const {
	if(this == gethuman())
		return "Вы";
	return getname();
}

int	playeri::getinitiative() const {
	auto result = getinitiative(strategy);
	if(is(BonusInitiative))
		result = 0;
	return result;
}

playeri* playeri::getspeaker() {
	return speaker;
}

playeri* playeri::gethuman() {
	return human_player;
}

int	playeri::get(action_s id) const {
	int r = costi::get(id);
	switch(id) {
	case Fleet:
		if(is(BonusFleetTokens))
			r++;
		break;
	}
	return r;
}

int playeri::getinfluences() const {
	return planeti::get(this, &planeti::getresource);
}

int	playeri::getresources() const {
	return planeti::get(this, &planeti::getresource);
}

void playeri::getinfo(string& sb) const {
	sb.add("###%1", getname());
	if(strategy)
		sb.addn("[+%1 стратегия]", bsmeta<strategyi>::elements[strategy].name);
	sb.addn("%1i ресурсов", planeti::get(this, &planeti::getresource));
	sb.addn("%1i влияния", planeti::get(this, &planeti::getinfluence));
	sb.addn("%1i стратегических маркеров", get(Strategy));
	sb.addn("%1i тактических маркеров", get(Command));
	sb.addn("%1i маркеров флота", get(Fleet));
	sb.addn("%1i товаров", get(Goods));
	if(getspeaker() == this)
		sb.addn("Являются [спикером] сената.");
	if(gethuman() == this)
		sb.addn("Это [ваша] нация.");
}

void playeri::setup() {
	create_action_deck();
	speaker = &bsmeta<playeri>::elements[rand() % (sizeof(bsmeta<playeri>::elements) / sizeof(bsmeta<playeri>::elements[0]))];
	planeti::setup();
	for(auto& e : bsmeta<playeri>())
		create_start_units(&e);
	uniti::update_control();
}

static void select(playera& source, const playeri* start) {
	auto index = start->getid();
	for(auto& e : bsmeta<playeri>()) {
		source.add(&bsmeta<playeri>::elements[index++]);
		if(index >= (int)bsmeta<playeri>::count)
			index = 0;
	}
}

static void strategic_phase() {
	playera source;
	select(source, playeri::getspeaker());
	for(auto p : source)
		p->strategy = NoStrategy;
	answeri ai;
	adat<strategy_s, Imperial + 1> politics;
	for(auto i = Leadership; i <= Imperial; i = (strategy_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		ai.clear();
		p->activate();
		if(!p->iscomputer()) {
			for(auto e : politics)
				ai.add(e, getstr(e));
			ai.sort();
			p->strategy = (strategy_s)ai.choose(
				"Эта стратегическая фаза. Вам нужно выбрать одну стратегию из списка ниже, которую будете использовать на этот ход. Будьте внимательны. Ваши враги также выбирают одну стратегию из этого же списка.", p);
		} else {
			p->strategy = politics.data[rand() % politics.getcount()];
			string sb;
			sb.adds("Наш выбор [%-1] стратегия.", getstr(p->strategy));
			sb.adds(bsmeta<strategyi>::elements[p->strategy].text);
			p->message(sb);
		}
		politics.remove(politics.indexof(p->strategy));
	}
}

int playeri::getcardscount() const {
	auto result = 0;
	for(auto i = FirstActionCard; i <= LastActionCard; i = (action_s)(i + 1))
		result += get(i);
	return result;
}

void playeri::check_card_limin() {

}

action_s playeri::report(const string& sb) {
	answeri ai;
	ai.add(0, "Принять");
	return (action_s)ai.choosev(false, 0, gethuman()->id, sb);
}

playeri* playeri::choose_opponent(const char* text) {
	answeri ai;
	for(auto& e : bsmeta<playeri>()) {
		if(this == &e)
			continue;
		ai.add((int)&e, e.getname());
	}
	return (playeri*)ai.choose(text, this);
}

void playeri::add_action_cards(int value) {
	string sb;
	sb.add("%1 получили [%2i] карт действий.", getyouname(), value);
	auto show_card = true;
	for(auto i = 0; i < value; i++) {
		auto a = action_deck.draw();
		add(a, 1);
		if(!iscomputer()) {
			if(show_card) {
				sb.adds("Это");
				show_card = false;
			} else
				sb.adds(",");
			sb.adds(getstr(a));
		}
	}
	if(!iscomputer())
		sb.add(".");
	message(sb);
}

void playeri::add_command_tokens(int value) {
	static action_s command_area[] = {Strategy, Command, Fleet};
	for(auto i = 1; i <= value; i++) {
		string sb;
		answeri ai;
		sb.add("%1 получили %2i командных жетонов.", getyouname(), value);
		if(value==1)
			sb.adds("Куда хотите его распределить?");
		else
			sb.adds("Куда хотите распределить %1i из %2i жетон?", i, value);
		ai.clear();
		for(auto e : command_area)
			ai.add(e, "Жетон %1", getstr(e));
		auto a = (action_s)ai.choose(sb, this);
		add(a, 1);
	}
}

void playeri::buy_technology(int cost_resources) {
	string sb; answeri ai;
	sb.add("Если хотите, можете приобрести технологию за [%1i] очка ресурсов.", cost_resources);
	ai.clear();
	auto counter = 1;
	auto total = getresources();
	if(total >= counter * cost_resources)
		ai.add(counter, "Приобрести технологию за [%2i] ресурсов", counter*cost_resources);
	auto n = ai.choose(sb, this, true);
	if(!n)
		return;
	add_technology(1);
}

void playeri::buy_command_tokens(int cost_influence) {
	string sb; answeri ai;
	sb.add("Если хотите, можете приобрести жетоны команд за [%1i] очка влияния. Сколько хотите приобрести?", cost_influence);
	ai.clear();
	auto total = getinfluences();
	auto counter = 1;
	while(total > counter*cost_influence) {
		ai.add(counter, "%1i жетона за %2i влияния", counter, counter*cost_influence);
		counter++;
	}
	auto n = ai.choose(sb, this, true);
	if(!n)
		return;
	add_command_tokens(n);
}

int	playeri::getfleet() const {
	return get(Fleet);
}

uniti* playeri::gethomesystem() const {
	auto index = getid();
	return bsmeta<solari>::elements + 33 + index;
}

void playeri::build_units(int value) {
	army result;
	select(result, TargetPlanet | Friendly | DockPresent);
	auto planet = static_cast<planeti*>(choose(result, "Укажите планету, на которой будете строить"));
	if(!planet)
		return;
	auto solar = static_cast<solari*>(planet->get(TargetSystem));
	auto dock = planet->find(SpaceDock);
	auto dock_produce = 0;
	if(dock)
		dock_produce = dock->getproduce();
	if(iscomputer()) {

	} else {
		if(build(result, planet, solar, getresources(), getfleet(), 0, dock_produce, true))
			uniti::update_control();
	}
}

uniti* playeri::choose(army& source, const char* format) const {
	answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	if(!ai)
		return 0;
	return (uniti*)ai.choose(format, this);
}

static void refresh_players() {
	for(auto& e : bsmeta<playeri>()) {
		e.set(StrategyAction, 1);
		e.set(TacticalAction, 1);
		e.set(Pass, 1);
	}
}

static void strategy_primary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->add_command_tokens(3);
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		break;
	case Politics:
		p->choose_speaker(1);
		p->add_action_cards(2);
		p->predict_next_political_card(2);
		break;
	case Trade:
		p->add_trade_goods(3);
		p->add_profit_for_trade_agreements();
		p->open_trade_negatiation();
		break;
	case Warfare:
		p->return_command_from_board(1);
		break;
	case Technology:
		p->add_technology(1);
		p->buy_technology(6);
		break;
	case Imperial:
		p->add_objective(1);
		p->add_victory_points(2);
		break;
	}
}

static void strategy_secondanary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		p->refresh_planets(1);
		break;
	case Politics:
		p->add_action_cards(1);
		break;
	case Trade:
		p->add_profit_for_trade_agreements();
		break;
	case Warfare:
		break;
	case Technology:
		p->buy_technology(4);
		break;
	case Imperial:
		p->build_units(1);
		break;
	}
}

void playeri::tactical_action() {
	auto solar = choose_solar();
	solar->activate(this);
	moveships(solar);
}

static action_s choose_action(playeri* p) {
	string sb;
	answeri ai;
	sb.add("Что вы предпочитаете делать в свой ход?");
	for(auto a = Armistice; a <= LastAction; a = (action_s)(a + 1)) {
		if(!p->is(a) || !p->isallow(AsAction, a))
			continue;
		ai.add(a, getstr(a), getstr(p->strategy));
	}
	return (action_s)ai.choose(sb, p);
}

static void play_action(playeri* p, action_s id) {
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
			for(auto& e : bsmeta<playeri>()) {
				if(e.get(Pass) == 0)
					continue;
				if(e.getinitiative() != i)
					continue;
				e.activate();
				auto a = choose_action(&e);
				play_action(&e, a);
				e.add(a, -1);
				someone_move = true;
			}
		}
	}
}

void playeri::make_move(bool strategic, bool action) {
	if(strategic)
		strategic_phase();
	if(action)
		action_phase();
}

void playeri::slide(const uniti* p) {
	if(!p)
		return;
	auto index = p->getindex();
	if(index == Blocked)
		return;
	slide(index);
}

unsigned playeri::select(uniti** result, uniti* const* pe, unsigned flags, group_s type) const {
	auto ps = result;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(flags&Friendly && e.getplayer() != this)
			continue;
		if(e.type != SpaceDock)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - result;
}

void playeri::select(army& result, unsigned flags) const {
	if(flags&DockPresent) {
		result.count = select(result.begin(), result.endof(), flags, SpaceDock);
		result.transform((target_s)(flags&TargetMask));
		result.rollup();
		return;
	}
	switch(flags&TargetMask) {
	case TargetSystem:
		for(auto& e : bsmeta<solari>()) {
			if(!e)
				continue;
			if(flags&Friendly && e.getplayer() != this)
				continue;
			result.add(&e);
		}
		break;
	case TargetPlanet:
		for(auto& e : bsmeta<planeti>()) {
			if(!e)
				continue;
			if(flags&Friendly && e.getplayer() != this)
				continue;
			result.add(&e);
		}
		break;
	}
}

void playeri::message(const char* text) {
	answeri ai;
	ai.add(0, "Принять");
	ai.choosev(false, 0, id, text);
}

void playeri::pay(int cost) {

}

void playeri::choose_speaker(int exclude) {
	string sb;
	sb.add("Вам необходимо назначить нового спикера. Старого спискера выбирать нельзя. Кто это будет?");
	answeri ai;
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(&e == speaker)
			continue;
		ai.add((int)&e, e.getname());
	}
	speaker = (playeri*)ai.choosev(false, 0, id, sb);
}