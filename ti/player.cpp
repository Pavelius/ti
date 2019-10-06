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
static player_pregen_info player_pregen_data[] = { { "xxcha", "Королевство Иксча", { 2, 3, 3 }, 4,
{ CombatBonusDefend },
{},
{ Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser },
{ GravitonLaserSystem } },
{ "barony", "Баронство Летнева", { 2, 3, 3 }, 2,
{ BonusFleetTokens },
{},
{ Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces },
{ AntimassDeflectors, PlasmaScoring }
},
{ "naalu", "Община Наалу", { 2, 3, 3 }, 3,
{ BonusInitiative, CombatBonusFighters },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters },
{ SarweenTools, NeuralMotivator }
},
{ "mindnet", "Сеть раума L1z1x", { 2, 3, 3 }, 2,
{ BonusCostDreadnought, BonusCostDreadnought, CombatBonusGroundForcesAttack },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS },
{ NeuralMotivator, PlasmaScoring }
},
{ "yssaril", "Племена Изарилов", { 2, 3, 3 }, 3,
{ BonusActionCards },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS },
{ NeuralMotivator }
},
{ "sol", "Федерация Солнца", { 2, 3, 3 }, 4,
{ BonusCommandCounter },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer },
{ NeuralMotivator, AntimassDeflectors }
},
{ "mentax", "Коалиция Ментаков", { 2, 3, 3 }, 2,
{},
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS },
{ SarweenTools, PlasmaScoring }
},
{ "hacan", "Эмираты Хакканов", { 2, 3, 3 }, 6,
{ BonusTrade },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters },
{ AntimassDeflectors, SarweenTools }
},
{ "jelnar", "Университеты Джолнаров", { 2, 3, 3 }, 4,
{ CombatPenalty, BonusTechnology },
{},
{ GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought },
{ AntimassDeflectors, NeuralMotivator, SarweenTools, PlasmaScoring }
},
{ "norr", "Сардак Норры", { 2, 3, 3 }, 3,
{ CombatBonusAll },
{},
{ GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, PDS },
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

playeri* playeri::get(const char* id) {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
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
	int r;
	switch(id) {
	case Resource: r = getresources(); break;
	case Influence: r = getinfluences(); break;
	default: r = costi::get(id); break;
	}
	switch(id) {
	case Fleet:
		if(is(BonusFleetTokens))
			r++;
		break;
	}
	return r;
}

int playeri::getinfluences() const {
	return planeti::get(this, &planeti::getresource, Ready);
}

int	playeri::getresources() const {
	return planeti::get(this, &planeti::getresource, Ready);
}

void playeri::getinfo(string& sb) const {
	sb.add("###%1", getname());
	if(strategy)
		sb.addn("[+%1 стратегия]", bsmeta<strategyi>::elements[strategy].name);
	sb.addn("%1i ресурсов", getresources());
	sb.add(", %1i влияния", getinfluences());
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
				}
				else if(i == value - 1)
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
	static action_s command_area[] = { Strategic, Fleet, Tactical };
	string sb;
	sb.add("%1 получили [%2i] командных жетона.", getyouname(), value);
	if(iscomputer()) {
		message(sb);
	}
	else {
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

static const char* getcurrencynameof(action_s currency) {
	switch(currency) {
	case Strategic: return "стратегических жетонов";
	case Resource: return "ресурсов";
	case Influence: return "влияния";
	default: return "непонятно чего";
	}
}

static int getcn(action_s currency) {
	switch(currency) {
	case Strategic: return 0;
	case Resource: return 1;
	case Influence: return 2;
	default: return 3;
	}
}

static const char* getcn(action_s currency, int count) {
	static const char* names[][3] = { { "стратегический жетон", "стратегических жетона", "стратегических жетонов" },
	{ "ресурс", "ресурса", "ресурсов" },
	{ "влияние", "влияния", "влияния" },
	{ "непонятно что", "непонятно чего", "непонятно чего" },
	};
	auto i = getcn(currency);
	if(count == 0 || count >= 5)
		count = 2;
	else if(count == 1)
		count = 0;
	else
		count = 1;
	return names[i][count];
}

void playeri::pay_apply(int cost, action_s id) {
	if(!cost)
		return;
	if(id != Resource && id != Influence) {
		add(id, -cost);
		return;
	}
	auto payed = 0;
	while(payed < cost) {
		string sb; answeri ai;
		sb.adds("Вам надо заплатить %1i %2.", cost, getcn(id, cost));
		sb.adds("Вы заплатили [%1i].", payed);
		sb.adds("Чем хотите заплатить?");
		planeta planets; select(planets, Friendly | Ready);
		for(auto p : planets) {
			auto c = p->get(id);
			if(!c)
				continue;
			ai.add((int)p, "%1: %2i", p->getname(), c);
		}
		auto goods = get(Goods);
		auto goods_koeff = 1;
		auto need_goods = (cost - payed + goods_koeff - 1) / goods_koeff;
		if(need_goods && need_goods <= goods) {
			if(goods_koeff > 1)
				ai.add(0, "Товары: %1ix%2i", need_goods, goods_koeff);
			else
				ai.add(0, "Товары: %1i", need_goods);
		}
		auto r = (planeti*)choose(ai, false, sb);
		if(!r) {
			payed += need_goods*goods_koeff;
			add(Goods, -need_goods);
		}
		else {
			payed += r->get(id);
			r->set(Exhaused);
		}
	}
}

int playeri::pay_choose(int maximum, int cost, const char* subject, const char* subjects, action_s currency) const {
	string sb; answeri ai;
	auto counter = 1;
	auto total = get(currency);
	if(currency == Influence || currency == Resource)
		total += get(Goods);
	if(!subjects)
		subjects = subject;
	while(counter <= maximum) {
		auto total_cost = counter * cost;
		if(total >= total_cost) {
			if(counter == 1) {
				if(total_cost == 1)
					ai.add(counter, "Купить %1 за %2", subject, getcn(currency, total_cost));
				else
					ai.add(counter, "Купить %1 за %2i %3", subject, total_cost, getcn(currency, total_cost));
			}
			else
				ai.add(counter, "Купить %3i %1 за %2i %3", subjects, total_cost, counter, getcn(currency, total_cost));
		}
		counter++;
	}
	if(cost == 1)
		sb.add("Если хотите, можете купить %2 за %3.", cost, subject, getcn(currency, cost));
	else
		sb.add("Если хотите, можете купить %2 за [%1i] %3.", cost, subject, getcn(currency, cost));
	return choose(ai, true, sb);
}

int playeri::pay(int maximum, int cost, const char* subject, const char* subjects, action_s currency) {
	auto n = pay_choose(maximum, cost, subject, subjects, currency);
	if(n)
		pay_apply(n*cost, currency);
	return n;
}

void playeri::buy_technology(int cost_resources) {
	auto n = pay(1, cost_resources, "технологию", "технологии", Resource);
	if(!n)
		return;
	add_technology(1);
}

void playeri::buy_command_tokens(int cost_influence) {
	auto n = pay(1, cost_influence, "коммандный жетон", "коммандных жетона", Influence);
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

void playeri::build_units(solari* solar) {
	if(!solar)
		return;
	planeta result; solar->select(result, Friendly | DockPresent);
	auto planet = choose(result, "Укажите планету, на которой будете строить");
	if(!planet)
		return;
	builda a1(this);
	int used_resources = 0;
	if(iscomputer()) {

	}
	else {
		if(!build(a1, planet, solar, getresources(), used_resources, getfleet(), 0, planet->getproduction(), true))
			return;
	}
	pay_apply(used_resources, Resource);
	for(auto& e : a1) {
		for(auto i = e.count * bsmeta<varianti>::elements[e.type].production; i > 0; i--) {
			if(e.isplanetary())
				create(e.type, planet);
			else
				create(e.type, solar);
		}
	}
}

uniti* playeri::choose(unita& source, const char* format) const {
	answeri ai;
	for(auto p : source)
		ai.add((int)p, p->getname());
	return (uniti*)choose(ai, false, format);
}

void playeri::choose_diplomacy() {
	solara source; selectp(source, Friendly | NoMekatol);
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
	apply(sb);
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

void playeri::selecta(solara& result) const {
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		auto g = e.getgroup();
		if(g == Supernova || g == AsteroidField)
			continue;
		if(e.isactivated(this))
			continue;
		result.add(&e);
	}
}

void playeri::selectp(solara& result, unsigned flags) const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		auto s = e.getsolar();
		if(!s)
			continue;
		if((flags&NoMekatol) != 0 && s == bsmeta<solari>::elements)
			continue;
		if((flags&Friendly) != 0 && e.getplayer() != this)
			continue;
		if((flags&Activated) != 0 && !e.is(Exhaused))
			continue;
		if(!result.is(s))
			result.add(s);
	}
}

void playeri::select(planeta& result, unsigned flags) const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if((flags&Friendly) != 0 && e.getplayer() != this)
			continue;
		if((flags&Activated) != 0 && !e.is(Exhaused))
			continue;
		if((flags&Ready) != 0 && e.is(Exhaused))
			continue;
		if((flags&DockPresent) != 0 && e.getcount(SpaceDock, this) == 0)
			continue;
		result.add(&e);
	}
}

void playeri::message(const char* text) {
	answeri ai;
	ai.add(1, "Принять");
	ai.choose(false, false, 0, id, text);
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

void playeri::apply(const char* format) {
	answeri ai;
	ai.add(1, "Принять");
	ai.choose(false, false, 0, id, format);
}

planeti* playeri::choose(const aref<planeti*>& source, const char* format) const {
	if(source.getcount() == 1)
		return source[0];
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
			}
			else
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

void playeri::add_victory_points(int value) {
	string sb;
	sb.adds("Мы получаем [%1] победное очко.");
	sb.adds("Победа близка, галактика скоро будет нашей.");
	apply(sb);
	add(VictoryPoints, value);
}

int	playeri::getcount(variant_s unit) const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getplayer() != this)
			continue;
		result++;
	}
	return result;
}