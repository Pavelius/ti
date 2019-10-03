#include "main.h"

solari					bsmeta<solari>::elements[48];
unsigned				bsmeta<solari>::count = sizeof(bsmeta<solari>::elements) / sizeof(bsmeta<solari>::elements[0]);
static unsigned char	solar_indecies[map_scan_line * map_scan_line];
static unsigned char	movement_rate[map_scan_line * map_scan_line];

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
	variant_s	type;
	char		from;
	char		to;
};
static solar_range solar_range_data[] = {{Solar, 21, 24},
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

int planeti::get(const playeri* player, int(planeti::*getproc)() const) {
	auto result = 0;
	for(auto& e : bsmeta<planeti>()) {
		if(e.getplayer() != player)
			continue;
		result += (e.*getproc)();
	}
	return result;
}

void planeti::refresh() {
	for(auto& e : bsmeta<planeti>())
		e.flags = 0;
}

void planeti::initialize() {
	for(auto& e : bsmeta<solari>())
		e.set(Solar);
	for(auto& e : solar_range_data) {
		for(auto i = e.from; i <= e.to; i++)
			bsmeta<solari>::elements[i].set(e.type);
	}
	for(auto& e : bsmeta<planeti>()) {
		e.player = 0xFF;
		e.flags = 0;
	}
}

void planeti::setup() {
	for(auto& e : bsmeta<planeti>()) {
		if(!e.home)
			continue;
		e.setplayer(playeri::find(e.home));
		e.set(PlanetUsed);
	}
}

int get_system_count() {
	auto result = 0;
	for(auto e : solar_indecies) {
		if(e == Blocked)
			continue;
		result++;
	}
	return result;
}

void planeti::create_stars() {
	char player_pos[][2] = {{3, 0}, {6, 0}, {0, 3}, {6, 3}, {0, 6}, {3, 6}};
	memset(solar_indecies, 0, sizeof(solar_indecies));
	for(auto& e : bsmeta<solari>()) {
		e.setplayer(0);
		e.setindex(0xFF);
	}
	for(auto y = 0; y < 7; y++) {
		if(y < 3) {
			for(auto x = 0; x < (3 - y); x++)
				solar_indecies[gmi(x, y)] = Blocked;
		} else {
			for(auto x = 7 - (y - 3); x < 8; x++)
				solar_indecies[gmi(x, y)] = Blocked;
		}
		solar_indecies[gmi(7, y)] = Blocked;
	}
	for(auto x = 0; x < 8; x++)
		solar_indecies[gmi(x, 7)] = Blocked;
	adat<int, 32> solar_deck;
	for(auto i = 1; i <= 32; i++)
		solar_deck.add(i);
	zshuffle(solar_deck.data, solar_deck.count);
	solar_indecies[gmi(3, 3)] = Blocked; // Мекатор рекс
	for(auto e : player_pos)
		solar_indecies[gmi(e[0], e[1])] = Blocked;
	auto allowed_system = get_system_count();
	auto index = 0;
	for(auto& e : solar_indecies) {
		if(e == Blocked)
			continue;
		if(index < solar_deck.getcount())
			e = solar_deck.data[index++];
	}
	// Расположили Менкатол Рекс
	solar_indecies[gmi(3, 3)] = 0;
	// Расопложим игроков
	for(auto& pn : bsmeta<planeti>()) {
		if(!pn.home)
			continue;
		pn.setsolar(0);
		pn.remove(PlanetUsed);
	}
	index = 33;
	int player_index = 0;
	for(auto& e : bsmeta<playeri>()) {
		for(auto& pn : bsmeta<planeti>()) {
			if(!pn.home)
				continue;
			if(strcmp(pn.home, e.id) == 0)
				pn.setsolar(bsmeta<solari>::elements + index);
		}
		solar_indecies[gmi(player_pos[player_index][0], player_pos[player_index][1])] = index;
		player_index++;
		index++;
	}
	// Обновим индексы звездных планет 
	for(unsigned i = 0; i < sizeof(solar_indecies) / sizeof(solar_indecies[0]); i++) {
		auto s = solar_indecies[i];
		if(s == Blocked)
			continue;
		auto& e = bsmeta<solari>::elements[s];
		e.setindex(i);
		for(auto& u : bsmeta<planeti>()) {
			if(u.getsolar() == &e)
				u.set(PlanetUsed);
		}
	}
}

int uniti::getmovement(short unsigned index) {
	if(index == Blocked)
		return 0;
	return movement_rate[index];
}

planeti* uniti::getplanet() const {
	if(parent.type == Planet)
		return parent.getplanet();
	return 0;
}

solari* solari::getsolar(short unsigned index) {
	auto n = solar_indecies[index];
	if(n == Blocked)
		return 0;
	return bsmeta<solari>::elements + n;
}

enum direction_s : unsigned char {
	LeftUp, RightUp, Left, Right, LeftDown, RightDown
};

static unsigned char getmovement(unsigned char index, direction_s d) {
	if(index == Blocked)
		return Blocked;
	auto x = gmx(index);
	auto y = gmy(index);
	switch(d) {
	case Left: x--; break;
	case Right: x++; break;
	case LeftUp: y--; break;
	case LeftDown: y++; x--; break;
	case RightUp: y--; x++; break;
	case RightDown: y++; break;
	default: return Blocked;
	}
	if(x < 0 || x >= map_scan_line || y < 0 || y >= map_scan_line)
		return Blocked;
	auto i = gmi(x, y);
	if(solar_indecies[i] == Blocked)
		return Blocked;
	return i;
}

static void make_wave(unsigned char start_index, const playeri* player, unsigned char* result, bool block) {
	static direction_s directions[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};
	unsigned char stack[256 * 8];
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
		auto p = solari::getsolar(index);
		if(!p)
			continue;
		auto p_player = p->getplayer();
		auto allow_movement = true;
		auto type = p->getgroup();
		if(type == Nebula)
			allow_movement = false;
		else if(p_player && p_player->isenemy(player) && !p_player->is(LightWaveDeflector))
			allow_movement = false;
		if(allow_movement) {
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
}

static void make_wave(unsigned char start_index, const playeri* player) {
	for(auto& e : movement_rate)
		e = DefaultCost;
	for(auto& e : bsmeta<solari>()) {
		if(!e)
			continue;
		auto type = e.getgroup();
		if(type == Solar || type == Nebula)
			continue;
		if(type == AsteroidField && player->is(AntimassDeflectors))
			continue;
		movement_rate[e.getindex()] = Blocked;
	}
	make_wave(start_index, player, movement_rate, false);
}

static void select_units(army& a1, const playeri* player) {
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.getplayer() != player)
			continue;
		auto solar = e.getsolar();
		if(!solar)
			continue;
		auto move_cost = e.getmovement(solar->getindex());
		if(e.getmovement() < move_cost)
			continue;
		a1.add(&e);
	}
}

void playeri::moveships(solari* solar) {
	army a1, a2;
	make_wave(solar->getindex(), this);
	select_units(a1, this);
	// Перемещение кораблей в систему
	if(choose(a1, a2, "Переместить", true)) {
		for(auto p : a2)
			p->setsolar(solar);
	}
	// Берем под контроль нейтральную систему
	if(!solar->getplayer())
		solar->setplayer(this);
}

planeti* planeti::find(const solari* parent, int index) {
	if(!index)
		return 0;
	for(auto& e : bsmeta<planeti>()) {
		if(e.getsolar() == parent) {
			if(--index == 0)
				return &e;
		}
	}
	return 0;
}

int	planeti::getcount(variant_s type, const playeri* player) const {
	auto result = 0;
	for(auto& e : bsmeta<uniti>()) {
		if(!e)
			continue;
		if(e.type == type && e.getplayer() == player && e.getplanet()==this)
			result++;
	}
	return result;
}