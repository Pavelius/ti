#include "main.h"

DECLBASE(solari, 48);
static int		solar_indecies[map_scan_line * map_scan_line];
short unsigned	movement_rate[48];

planeti bsmeta<planeti>::elements[] = {{"Архон рен", "xxcha", 2, 3, 0},
{"Арк Прайм", "barony", 4, 0, 0},
{"Мейлук", "naalu", 0, 2, 0},
{"[0.0.0]", "mindnet", 5, 0, 0},
{"Ретиллион", "yssaril", 2, 3, 0},
{"Йорд", "sol", 4, 2, 0},
{"Мол Примус", "mentak", 4, 1, 0},
{"Аретц", "hacan", 2, 0, 0},
{"Нар", "jelnar", 2, 1, 0},
{"Трен'Лак", "norr", 1, 0, 0},
//
{"Архон Тау", "xxcha", 1, 1, 5},
{"Врен Терра", "barony", 2, 1, 5},
{"Друа", "naalu", 3, 1, 5},
{"Шаллок", "yssaril", 1, 2, 5},
{"Херкант", "hacan", 1, 1, 5},
{"Жол", "jelnar", 1, 2, 5},
{"Куинарра", "norr", 3, 1, 5},
//
{"Камдорн", "hacan", 0, 1, 2},
//
{"Мекатол Рекс", 0, 1, 6, 16, NoTech},
{"Берег", 1, 3, 1, 12, Green},
{"Центаури", 2, 1, 3, 13, Blue},
{"Квуцен'н", 3, 1, 2, 2, Green},
{"Текю'Ран", 4, 2, 0, 3, Red},
{"Новый Альбион", 5, 1, 1, 4, Green},
{"Курник", 6, 1, 2, 0, Red},
{"Меллоун", 7, 0, 2, 5, NoTech},
{"Лазар", 8, 1, 0, 6, NoTech},
{"Арнор", 9, 2, 1, 7, NoTech},
{"Аринам", 10, 1, 2, 8, Blue},
{"Абиз", 11, 3, 0, 9, NoTech},
{"Дал Бутта", 12, 0, 2, 8, Red},
{"Заудрон", 13, 2, 2, 10, NoTech},
{"Тибах", 14, 1, 1, 11, NoTech},
{"Мехар Ксулл", 15, 1, 3, 12, Blue},
{"Тар'Ман", 16, 1, 1, 2, NoTech},
{"Веллон", 17, 1, 2, 13, NoTech},
{"Вифут II", 18, 2, 0, 14, Red},
{"Лодор", 19, 3, 1, 15, Green, WormholeAlpha},
{"Квен", 20, 2, 1, 0, Green, WormholeBeta},
{"Лирта IV", 1, 2, 3, 2, NoTech},
{"Грал", 2, 1, 1, 3, NoTech},
{"Рарун", 3, 0, 3, 5, NoTech},
{"Торкан", 4, 0, 3, 6, Blue},
{"Старпоинт", 5, 3, 1, 7, NoTech},
{"Рескулон", 6, 2, 0, 0, NoTech},
{"Зохбат", 7, 3, 1, 14, Blue},
{"Сакулаг", 8, 2, 1, 2, NoTech},
{"Лор", 9, 1, 2, 3, Red},
{"Мир", 10, 0, 4, 6, NoTech},
{"Файра", 11, 2, 0, 0, Blue},
{"Ксехан", 12, 1, 1, 2, Green},
};
unsigned bsmeta<planeti>::count = sizeof(bsmeta<planeti>::elements) / sizeof(bsmeta<planeti>::elements[0]);
const unsigned bsmeta<planeti>::count_maximum = sizeof(bsmeta<planeti>::elements) / sizeof(bsmeta<planeti>::elements[0]);
struct solar_range {
	group_s		type;
	char		from;
	char		to;
};
static solar_range solar_range_data[] = {{SolarSystem, 21, 24},
{AsteroidField, 25, 28},
{Nebula, 29, 30},
{Supernova, 31, 32},
};

int	planeti::getinfluence() const {
	return influence;
}

int	planeti::getresource() const {
	return resource;
}

unsigned planeti::select(planeti** result, planeti* const*result_max, const char* home) {
	auto p = result;
	for(auto& e : bsmeta<planeti>()) {
		if(!e.home)
			continue;
		if(strcmp(e.home, home) != 0)
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

unsigned planeti::select(planeti** result, planeti* const* result_max, uniti* parent) {
	auto p = result;
	for(auto& e : bsmeta<planeti>()) {
		if(e.parent != parent)
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

int planeti::get(const playeri* player, int(planeti::*getproc)() const) {
	auto result = 0;
	for(auto& e : bsmeta<planeti>()) {
		if(e.player != player)
			continue;
		result += (e.*getproc)();
	}
	return result;
}

void planeti::refresh() {
	for(auto& e : bsmeta<planeti>())
		e.activate_flags = 0;
}

void planeti::initialize() {
	for(auto& e : bsmeta<solari>())
		e.type = SolarSystem;
	for(auto& e : solar_range_data) {
		for(auto i = e.from; i <= e.to; i++)
			bsmeta<solari>::elements[i].type = e.type;
	}
	for(auto& e : bsmeta<planeti>()) {
		e.type = Planet;
		e.player = 0;
		if(e.solar == -1)
			e.parent = 0;
		else
			e.parent = bsmeta<solari>::elements + e.solar;
	}
}

void planeti::setup() {
	for(auto& e : bsmeta<planeti>()) {
		if(!e.home)
			continue;
		e.player = playeri::find(e.home);
	}
}

int get_system_count() {
	auto result = 0;
	for(auto e : solar_indecies) {
		if(e == -1)
			continue;
		result++;
	}
	return result;
}

void planeti::create_stars() {
	char player_pos[][2] = {{3, 0}, {6, 0}, {0, 3}, {6, 3}, {0, 6}, {3, 6}};
	memset(solar_indecies, 0, sizeof(solar_indecies));
	for(auto y = 0; y < 7; y++) {
		if(y < 3) {
			for(auto x = 0; x < (3 - y); x++)
				solar_indecies[gmi(x, y)] = -1;
		} else {
			for(auto x = 7 - (y - 3); x < 8; x++)
				solar_indecies[gmi(x, y)] = -1;
		}
		solar_indecies[gmi(7, y)] = -1;
	}
	for(auto x = 0; x < 8; x++)
		solar_indecies[gmi(x, 7)] = -1;
	adat<int, 32> solar_deck;
	for(auto i = 1; i <= 32; i++)
		solar_deck.add(i);
	zshuffle(solar_deck.data, solar_deck.count);
	solar_indecies[gmi(3, 3)] = -1; // Мекатор рекс
	for(auto e : player_pos)
		solar_indecies[gmi(e[0], e[1])] = -1;
	auto allowed_system = get_system_count();
	auto index = 0;
	for(auto& e : solar_indecies) {
		if(e == -1)
			continue;
		if(index < solar_deck.getcount())
			e = solar_deck[index++];
	}
	solar_indecies[gmi(3, 3)] = 0; // Расположили Менкатол Рекс
	// Расопложим игроков
	index = 33;
	int player_index = 0;
	for(auto& e : bsmeta<playeri>()) {
		for(auto& pn : bsmeta<planeti>()) {
			if(!pn.home)
				continue;
			if(strcmp(pn.home, e.id) == 0)
				pn.parent = bsmeta<solari>::elements + index;
		}
		solar_indecies[gmi(player_pos[player_index][0], player_pos[player_index][1])] = index;
		player_index++;
		index++;
	}
}

int uniti::getmovement(short unsigned index) {
	if(index == Blocked)
		return 0;
	return movement_rate[index];
}

bool uniti::issolar() const {
	return this >= bsmeta<solari>::elements &&
		this < (bsmeta<solari>::elements + bsmeta<solari>::count);
}

short unsigned uniti::getsolarindex() const {
	if(issolar())
		return static_cast<const solari*>(this) - bsmeta<solari>::elements;
	return Blocked;
}

bool uniti::isplanet() const {
	return this >= bsmeta<planeti>::elements
		&& this < (bsmeta<planeti>::elements + bsmeta<planeti>::count);
}

planeti* uniti::getplanet() {
	if(isplanet())
		static_cast<planeti*>(this);
	return 0;
}

uniti* uniti::getsolar(int index) {
	auto n = solar_indecies[index];
	if(n == -1)
		return 0;
	return bsmeta<solari>::elements + n;
}

short unsigned uniti::getindex() const {
	auto index = getsolarindex();
	if(index != Blocked) {
		for(unsigned i = 0; i < sizeof(solar_indecies) / sizeof(solar_indecies[0]); i++) {
			if(solar_indecies[i] == index)
				return i;
		}
	}
	return Blocked;
}

const char* uniti::getsolarname() const {
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.parent == this)
			return e.name;
	}
	return getstr(type);
}

const char* uniti::getplanetname() const {
	return (static_cast<const planeti*>(this))->name;
}

enum direction_s : unsigned char {
	LeftUp, RightUp, Left, Right, LeftDown, RightDown
};

static short unsigned getmovement(short unsigned index, direction_s d) {
	if(index == Blocked)
		return Blocked;
	auto x = uniti::gmx(index);
	auto y = uniti::gmy(index);
	switch(d) {
	case Left: x--; break;
	case Right: x++; break;
	case LeftUp: y--; break;
	case LeftDown: y++; x--; break;
	case RightUp: y--; x++; break;
	case RightDown: y++; break;
	default: return Blocked;
	}
	if(x < 0 || x >= map_scan_line || y<0 || y>= map_scan_line)
		return Blocked;
	return uniti::gmi(x, y);
}

static void make_wave(short unsigned start_index, const playeri* player, short unsigned* result, bool block) {
	const int cost_bocked = 4;
	static direction_s directions[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};
	short unsigned stack[256 * 8];
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	result[start_index] = 0;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = result[index] + 1;
		auto p = uniti::getsolar(index);
		if(p->type == Nebula)
			cost += cost_bocked;
		else if(p->player && p->player->isenemy(player) && !player->is(LightWaveDeflector))
			cost += cost_bocked;
		for(auto d : directions) {
			auto i1 = getmovement(index, d);
			if(i1 == Blocked || result[i1] == Blocked)
				continue;
			if(result[i1] < cost)
				continue;
			result[i1] = cost;
			*push_counter++ = i1;
			if(push_counter >= stack_end)
				push_counter = stack;
		}
	}
}

static void make_wave(short unsigned start_index, const playeri* player) {
	for(auto& e : movement_rate)
		e = DefaultCost;
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		if(e.type == SolarSystem)
			continue;
		if(e.type == Nebula)
			continue;
		if(e.type == AsteroidField && player->is(AntimassDeflectors))
			continue;
		auto index = e.getindex();
		movement_rate[index] = Blocked;
	}
	make_wave(start_index, player, movement_rate, false);
}

static void select_units(army& a1, const playeri* player) {
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.player != player)
			continue;
		if(!e.isfleet())
			continue;
		auto move_cost = e.getmovement(e.parent->getindex());
		if(e.getmovement() < move_cost)
			continue;
		a1.add(&e);
	}
}

void playeri::moveships(uniti* solar) {
	army a1, a2;
	make_wave(solar->getindex(), this);
	select_units(a1, this);
	// Перемещение кораблей в систему
	if(choose(a1, a2, "Переместить", true)) {
		for(auto p : a2)
			p->parent = solar;
	}
	// Берем под контроль нейтральную систему
	if(!solar->player)
		solar->player = this;
}

planeti* planeti::find(const uniti* parent, int index) {
	if(!index)
		return 0;
	for(auto& e : bsmeta<planeti>()) {
		if(e.parent == parent) {
			if(--index == 0)
				return &e;
		}
	}
	return 0;
}