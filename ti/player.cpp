#include "main.h"

DECLBASE(playeri, 6);
struct player_pregen_info {
	const char*			id;
	const char*			name;
	char				tokens[3];
	char				commodities;
	cflags<bonus_s>		bonus;
	adat<action_s, 8>	actions;
	variant_s			start_units[16];
	cflags<tech_s>		start_tech;
};
static player_pregen_info player_pregen_data[] = {{"xxcha", "Королевство Иксча", {2, 3, 3}, 4,
{CombatBonusDefend},
{ExecutePrimaryAbility, ChangePoliticCard},
{Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser},
{AntimassDeflectors, EnviroCompensator}},
{"barony", "Баронство Летнева", {2, 3, 3}, 2,
{BonusFleetTokens},
{BaronyEquipment},
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{HylarVAssaultLaser, AntimassDeflectors}
},
{"naalu", "Община Наалу", {2, 3, 3}, 3,
{BonusInitiative, CombatBonusFighters},
{NaaluFleetRetreat},
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{EnviroCompensator, AntimassDeflectors}
},
{"mindnet", "Сеть раума L1z1x", {3, 3, 3}, 2,
{BonusCostDreadnought, BonusCostDreadnought, CombatBonusGroundForcesAttack},
{},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{EnviroCompensator, StasisCapsules, Cybernetics, HylarVAssaultLaser}
},
{"yssaril", "Племена Изарилов", {2, 3, 3}, 3,
{BonusActionCards},
{LookActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{Cybernetics, AntimassDeflectors}
},
{"sol", "Федерация Солнца", {2, 3, 3}, 4,
{BonusCommandCounter},
{SolOrbitalDrop},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{AntimassDeflectors, XRDTransporters}
},
{"mentax", "Коалиция Ментаков", {2, 3, 4}, 2,
{},
{MentakAmbush, MentakPiracy},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{EnviroCompensator, HylarVAssaultLaser}
},
{"hacan", "Эмираты Хакканов", {2, 3, 3}, 6,
{BonusTrade},
{HacanTradeActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{EnviroCompensator, SarweenTools}
},
{"jelnar", "Университеты Джолнаров", {2, 3, 3}, 4,
{CombatPenalty, BonusTechnology},
{JolanrRerollCombatDices},
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, HylarVAssaultLaser, EnviroCompensator, SarweenTools}
},
{"norr", "Сардак Норры", {2, 3, 3}, 3,
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
	return e2->getresource() - e1->getresource();
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

bool playeri::isallow(variant_s id) const {
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

uniti* playeri::create(variant_s id, solari* solar) {
	auto p = new uniti(id);
	p->setplayer(this);
	p->setsolar(solar);
	return p;
}

uniti* playeri::create(variant_s id, planeti* planet) {
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

static void update_control() {
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		auto player = e.getplayer();
		for(auto& u : bsmeta<uniti>()) {
			if(!u)
				continue;
			if(u.getsolar() != &e)
				continue;
			auto unit_player = u.getplayer();
			if(player == unit_player)
				continue;
			e.setplayer(unit_player);
			break;
		}
	}
}

static void create_start_units(playeri* player) {
	auto p = find_by_id(player->id);
	assert(p);
	planeta planets;
	for(auto& e : bsmeta<planeti>()) {
		auto ph = e.gethome();
		if(!ph)
			continue;
		if(strcmp(ph, p->id) == 0)
			planets.add(&e);
	}
	assert(planets.count);
	qsort(planets.data, planets.count, sizeof(planets.data[0]), compare_planets);
	auto base_planet = planets[0];
	auto solar_system = base_planet->getsolar();
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
	this->commodities = p->commodities;
	technologies = p->start_tech;
	bonuses = p->bonus;
	// Game setup: step 11
	set(Strategy, p->tokens[0]);
	set(Command, p->tokens[1]);
	set(Fleet, p->tokens[2]);
	set(Commodities, p->commodities);
	return *this;
}

unsigned char playeri::getid() const {
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
	sb.add(", %1i влияния", planeti::get(this, &planeti::getinfluence));
	sb.addn("%1i стратегических маркеров", get(Strategy));
	sb.addn("%1i тактических маркеров", get(Command));
	sb.addn("%1i маркеров флота", get(Fleet));
	sb.addn("%1i товаров, %2i продукции", get(Goods), get(Commodities));
	if(getspeaker() == this)
		sb.addn("Являются [спикером] сената.");
	if(gethuman() == this)
		sb.addn("Это [ваша] нация.");
}

void playeri::setup() {
	create_action_deck();
	create_agenda_deck();
	speaker = &bsmeta<playeri>::elements[rand() % (sizeof(bsmeta<playeri>::elements) / sizeof(bsmeta<playeri>::elements[0]))];
	planeti::setup();
	for(auto& e : bsmeta<playeri>())
		create_start_units(&e);
	update_control();
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
	adat<strategy_s, Imperial + 1> politics;
	for(auto i = Leadership; i <= Imperial; i = (strategy_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		string sb; sb.clear();
		answeri ai; ai.clear();
		p->activate();
		for(auto e : politics)
			ai.add(e, getstr(e));
		ai.sort();
		p->strategy = (strategy_s)p->choose(sb, ai, false,
			"Эта стратегическая фаза. "
			"Вам нужно выбрать одну стратегию из списка ниже, которую будете использовать на этот ход. "
			"Ваши оппоненты также выбирают одну стратегию из этого же списка.");
		sb.adds("Наш выбор [%-1] стратегия.", getstr(p->strategy));
		sb.adds(bsmeta<strategyi>::elements[p->strategy].text);
		p->message(sb);
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

playeri* playeri::choose_opponent(const char* text) {
	string sb;
	answeri ai;
	for(auto& e : bsmeta<playeri>()) {
		if(this == &e)
			continue;
		ai.add((int)&e, e.getname());
	}
	return (playeri*)choose(sb, ai, false, text);
}

void playeri::add_action_cards(int value) {
	string sb;
	sb.add("Мы получили [%2i] новых карт действий.", getyouname(), value);
	if(true) {
		auto first_card = true;
		for(auto i = 0; i < value; i++) {
			auto a = action_deck.draw();
			add(a, 1);
			if(!iscomputer()) {
				if(first_card) {
					sb.adds("Это");
					first_card = false;
				} else if(i == value - 1)
					sb.adds("и");
				else
					sb.add(",");
				sb.adds("[+");
				sb.add(getstr(a));
				sb.add("]");
			}
		}
	}
	if(!iscomputer())
		sb.add(".");
	message(sb);
}

void playeri::add_command_tokens(int value) {
	static action_s command_area[] = {Strategy, Command, Fleet};
	string sb;
	sb.add("%1 получили [%2i] командных жетона.", getyouname(), value);
	if(iscomputer()) {
		message(sb);
	} else {
		for(auto i = 1; i <= value; i++) {
			answeri ai; ai.clear();
			for(auto e : command_area)
				ai.add(e, "Жетон %1", getstr(e));
			auto a = (action_s)choose(sb, ai, false, "Куда хотите распределить %1i из %2i жетон?", i, value);
			add(a, 1);
		}
	}
}

void playeri::buy_technology(int cost_resources) {
	string sb; answeri ai;
	auto counter = 1;
	auto total = getresources();
	if(total >= counter * cost_resources)
		ai.add(counter, "Приобрести технологию за [%2i] ресурсов", counter*cost_resources);
	auto n = choose(sb, ai, true, "Если хотите, можете приобрести технологию за [%1i] очка ресурсов.", cost_resources);
	if(!n)
		return;
	add_technology(1);
}

void playeri::buy_command_tokens(int cost_influence) {
	string sb; answeri ai;
	auto total = getinfluences();
	auto counter = 1;
	while(total > counter*cost_influence) {
		ai.add(counter, "%1i жетона за %2i влияния", counter, counter*cost_influence);
		counter++;
	}
	auto n = choose(sb, ai, true,
		"Если хотите, можете приобрести жетоны команд за [%1i] очка влияния. "
		"Сколько хотите приобрести?", cost_influence);
	if(!n)
		return;
	add_command_tokens(n);
}

int	playeri::getfleet() const {
	return get(Fleet);
}

solari* playeri::gethomesystem() const {
	auto index = getid();
	return bsmeta<solari>::elements + 33 + index;
}

void playeri::build_units(int value) {
	planeta result;
	select(result, Friendly | DockPresent);
	auto planet = static_cast<planeti*>(choose(result, "Укажите планету, на которой будете строить"));
	if(!planet)
		return;
	auto solar = planet->getsolar();
	if(iscomputer()) {

	} else {
	}
}

uniti* playeri::choose(army& source, const char* format) const {
	string sb;
	answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	return (uniti*)choose(sb, ai, false, format);
}

static void refresh_players() {
	for(auto& e : bsmeta<playeri>()) {
		e.set(StrategyAction, 1);
		e.set(TacticalAction, 1);
		e.set(Pass, 1);
	}
}

void playeri::choose_diplomacy() {
	solara source; select(source, Friendly | NoMekatol);
	auto p = choose(source);
	if(!p)
		return;
	string sb;
	sb.add("Звездная система [%1] теперь имеет особый дипломатический статус. "
		"Любая активность в этой системе находится под особым надзором дипломатической миссии, которую мы возглавляем. "
		"Все игроки, кроме нас, ставят в эту систему жетон команд из сброса.",
		p->getname());
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(&e == this)
			continue;
		p->activate(&e);
	}
	auto count_planets = 0;
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.getsolar() != p)
			continue;
		if(e.getplayer() != this)
			continue;
		count_planets++;
		e.remove(Exhaused);
	}
	if(count_planets > 0)
		sb.adds("Наши планеты в этой зведной системе обновляют все свои ресурсы.");
	apply(sb);
}

static planeti* choose_planet_construct(const playeri* p, variant_s type, const char* format) {
	planeta source;
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.getplayer() != p)
			continue;
		if(type == PDS) {
			if(e.getcount(type, p) >= 2)
				continue;
		} else {
			if(e.getcount(type, p) >= 1)
				continue;
		}
		source.add(&e);
	}
	return p->choose(source, format, getstr(type));
}

static variant_s choose_dock_or_PDS(const playeri* p) {
	string sb; answeri ai;
	ai.add(SpaceDock, getstr(SpaceDock));
	ai.add(PDS, getstr(PDS));
	return (variant_s)p->choose(sb, ai, false, "Что вы хотите построить именно вы хотите построить в первую очередь?");
}

static void build_primary(playeri* p) {
	auto ut = choose_dock_or_PDS(p);
	auto pp = choose_planet_construct(p, ut, "Выбирайте планету, на которой будете строить [%1].");
	if(pp)
		p->create(ut, pp);
	pp = choose_planet_construct(p, PDS, "На втором шаге мы усилим свою безопасность. Где мы будем строить [%1]?");
	if(pp)
		p->create(PDS, pp);
}

void playeri::add_trade_goods(int value) {
	add(Goods, value);
	string sb;
	sb.add("Мы получили [%1i] товара. Товар можно использовать в любой момент для оплаты вместо влияния или ресурсов.", value);
	message(sb);
}

void playeri::replenish_commodities() {
	if(commodities == get(Commodities))
		return;
	set(Commodities, commodities);
	string sb;
	sb.add("Мы обновили свою продукцию и готовы распродать ее по самым лучшим ценам в галактике. На сейчас у нас имеется [%1i] единицы продукции.", get(Commodities));
	message(sb);
}

static void strategy_primary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->add_command_tokens(3);
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		p->choose_diplomacy();
		break;
	case Politics:
		p->choose_speaker(1);
		p->add_action_cards(2);
		p->predict_next_political_card(2);
		break;
	case Construction:
		build_primary(p);
		break;
	case Trade:
		p->add_trade_goods(3);
		p->replenish_commodities();
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
	adat<solari*, 64> source;
	aref<solari*> list;
	auto solar = choose(list);
	solar->activate(this);
	moveships(solar);
}

static action_s choose_action(playeri* p, play_s play) {
	string sb; answeri ai;
	for(auto a = Armistice; a <= LastAction; a = (action_s)(a + 1)) {
		if(!p->is(a) || !p->isallow(play, a))
			continue;
		ai.add(a, getstr(a), getstr(p->strategy));
	}
	return (action_s)p->choose(sb, ai, false, "Что вы предпочитаете делать в свой ход?");
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
				auto a = choose_action(&e, AsAction);
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

unsigned playeri::select(uniti** result, uniti* const* pe, unsigned flags, variant_s type) const {
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

void playeri::select(solara& result, unsigned flags) const {
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		if((flags&Friendly) != 0 && e.getplayer() != this)
			continue;
		if((flags&NoMekatol) != 0 && &e == bsmeta<solari>::elements)
			continue;
		result.add(&e);
	}
}

void playeri::select(planeta& result, unsigned flags) const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(flags&Friendly && e.getplayer() != this)
			continue;
		result.add(&e);
	}
}

void playeri::message(const char* text) {
	answeri ai;
	ai.add(1, "Принять");
	ai.choose(false, false, 0, id, text);
}

void playeri::pay(int cost) {

}

void playeri::choose_speaker(int exclude) {
	string sb;
	answeri ai;
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(&e == speaker)
			continue;
		ai.add((int)&e, e.getname());
	}
	speaker = (playeri*)choose(sb, ai, false, "Вам необходимо назначить нового спикера. Старого спикера выбирать нельзя. Кто это будет?");
	sb.add("Новым спикером в парламенте становятся [%1].", speaker->getname());
	apply(sb);
}

int playeri::choose(string& sb, answeri& ai, bool cancel, const char* format, ...) const {
	return choosev(sb, ai, cancel, format, xva_start(format));
}

void playeri::apply(string& sb) {
	answeri ai;
	ai.add(1, "Принять");
	ai.choose(false, false, 0, id, sb);
}

planeti* playeri::choose(const aref<planeti*>& source, const char* format, ...) const {
	string sb; answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	return (planeti*)choosev(sb, ai, false, format, xva_start(format));
}

int	playeri::choosev(string& sb, answeri& ai, bool cancel_button, const char* format, const char* format_param) const {
	auto p = sb.get();
	//sb.adds("[+");
	if(format)
		sb.addx(' ', format, format_param);
	//sb.add("]");
	auto r = ai.choose(cancel_button, iscomputer(), 0, id, sb);
	sb.set(p);
	return r;
}