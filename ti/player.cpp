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
{GravitonLaserSystem}},
{"barony", "Баронство Летнева", {2, 3, 3}, 2,
{BonusFleetTokens},
{BaronyEquipment},
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{AntimassDeflectors, PlasmaScoring}
},
{"naalu", "Община Наалу", {2, 3, 3}, 3,
{BonusInitiative, CombatBonusFighters},
{NaaluFleetRetreat},
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{SarweenTools, NeuralMotivator}
},
{"mindnet", "Сеть раума L1z1x", {3, 3, 3}, 2,
{BonusCostDreadnought, BonusCostDreadnought, CombatBonusGroundForcesAttack},
{},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{NeuralMotivator, PlasmaScoring}
},
{"yssaril", "Племена Изарилов", {2, 3, 3}, 3,
{BonusActionCards},
{LookActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{NeuralMotivator}
},
{"sol", "Федерация Солнца", {2, 3, 3}, 4,
{BonusCommandCounter},
{SolOrbitalDrop},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{NeuralMotivator, AntimassDeflectors}
},
{"mentax", "Коалиция Ментаков", {2, 3, 4}, 2,
{},
{MentakAmbush, MentakPiracy},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{SarweenTools, PlasmaScoring}
},
{"hacan", "Эмираты Хакканов", {2, 3, 3}, 6,
{BonusTrade},
{HacanTradeActionCards},
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{AntimassDeflectors, SarweenTools}
},
{"jelnar", "Университеты Джолнаров", {2, 3, 3}, 4,
{CombatPenalty, BonusTechnology},
{JolanrRerollCombatDices},
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, NeuralMotivator, SarweenTools, PlasmaScoring}
},
{"norr", "Сардак Норры", {2, 3, 3}, 3,
{CombatBonusAll},
{},
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, PDS},
{}
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
	set(Strategic, p->tokens[0]);
	set(Tactical, p->tokens[1]);
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
	sb.addn("%1i стратегических маркеров", get(Strategic));
	sb.addn("%1i маркеров флота", get(Fleet));
	sb.addn("%1i тактических маркеров", get(Tactical));
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

int playeri::getcardscount() const {
	auto result = 0;
	for(auto i = FirstActionCard; i <= LastActionCard; i = (action_s)(i + 1))
		result += get(i);
	return result;
}

void playeri::check_card_limin() {

}

playeri* playeri::choose_opponent(const char* text) {
	answeri ai;
	for(auto& e : bsmeta<playeri>()) {
		if(this == &e)
			continue;
		ai.add((int)&e, e.getname());
	}
	return (playeri*)choose(ai, false, text);
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
	static action_s command_area[] = {Strategic, Fleet, Tactical};
	string sb;
	sb.add("%1 получили [%2i] командных жетона.", getyouname(), value);
	if(iscomputer()) {
		message(sb);
	} else {
		for(auto i = 1; i <= value; i++) {
			answeri ai; ai.clear();
			for(auto e : command_area)
				ai.add(e, "Жетон %1", getstr(e));
			auto ps = sb.get();
			sb.adds("Куда хотите распределить %1i из %2i жетон?", i, value);
			auto a = (action_s)choose(ai, false, sb);
			sb.set(ps);
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
	sb.add("Если хотите, можете приобрести одну технологию за [%1i] очка ресурсов.", cost_resources);
	auto n = choose(ai, true, sb);
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
	sb.adds("Если хотите, можете приобрести жетоны команд за [%1i] очка влияния.", cost_influence);
	sb.adds("Сколько хотите приобрести?");
	auto n = choose(ai, true, sb);
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
	answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	return (uniti*)choose(ai, false, format);
}

void playeri::choose_diplomacy() {
	solara source; select(source, Friendly | NoMekatol);
	auto p = choose(source, "Выбирайте звездную систему, которая будет иметь особый дипломатический статус на весь этот ход.");
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
		if((flags&Activated) != 0 && !e.isactivated(this))
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
	auto ps = sb.get();
	sb.adds("Вам необходимо назначить нового спикера. Старого спикера выбирать нельзя. Кто это будет?");
	speaker = (playeri*)choose(ai, false, sb);
	sb.set(ps);
	sb.add("Новым спикером в парламенте становятся [%1].", speaker->getname());
	apply(sb);
}

void playeri::apply(string& sb) {
	answeri ai;
	ai.add(1, "Принять");
	ai.choose(false, false, 0, id, sb);
}

planeti* playeri::choose(const aref<planeti*>& source, const char* format) const {
	answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	return (planeti*)choose(ai, false, format);
}

int	playeri::choose(answeri& ai, bool cancel_button, const char* format) const {
	return ai.choose(cancel_button, iscomputer(), 0, id, format);
}

int get_tech_color_count(const playeri* p, tech_color_s c) {
	auto result = 0;
	for(auto i = FirstTech; i <= LastTech; i = tech_s(i + 1)) {
		if(bsmeta<techi>::elements[i].color != c)
			continue;
		if(!p->is(i))
			continue;
		result++;
	}
	return result;
}

bool playeri::isallow(tech_s v) const {
	auto& e = bsmeta<techi>::elements[v];
	auto free_requisit = 0;
	for(auto i = Red; i <= Yellow; i = tech_color_s(i + 1)) {
		auto r = e.required[i - Red];
		if(r == 0)
			continue;
		auto n = get_tech_color_count(this, i);
		if(n < r) {
			if(free_requisit > 0) {
				if(n < r - 1)
					return false;
				free_requisit--;
			} else
				return false;
		}
	}
	return true;
}

void playeri::add_technology(int value) {
	answeri ai;
	for(auto i = FirstTech; i <= LastTech; i = tech_s(i + 1)) {
		if(is(i))
			continue;
		if(!isallow(i))
			continue;
		ai.add(i, getstr(i));
	}
	auto t = (tech_s)choose(ai, false, "Мы можем изучить новую тезнологию. Какую технологию из доступных выбрать?");
	set(t);
	string sb;
	sb.add("Наши ученные изучили новую технологию [%1].", getstr(t));
	apply(sb);
}