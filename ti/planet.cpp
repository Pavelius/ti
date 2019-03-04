#include "main.h"

int			solar_map[8 * 8];
unit_info	solars[48];

static planet_info planets[] = {{"����� ���", "xxcha", 2, 3, 0},
{"��� �����", "barony", 4, 0, 0},
{"������", "naalu", 0, 2, 0},
{"[0.0.0]", "mindnet", 5, 0, 0},
{"���������", "yssaril", 2, 3, 0},
{"����", "sol", 4, 2, 0},
{"��� ������", "mentak", 4, 1, 0},
{"�����", "hacan", 2, 0, 0},
{"���", "jelnar", 2, 1, 0},
{"����'���", "norr", 1, 0, 0},
//
{"����� ���", "xxcha", 1, 1, 5},
{"���� �����", "barony", 2, 1, 5},
{"����", "naalu", 3, 1, 5},
{"������", "yssaril", 1, 2, 5},
{"�������", "hacan", 1, 1, 5},
{"���", "jelnar", 1, 2, 5},
{"��������", "norr", 3, 1, 5},
//
{"�������", "hacan", 0, 1, 2},
//
{"������� ����", 0, 1, 6, 16, NoTech},
{"�����", 1, 3, 1, 12, Green},
{"��������", 2, 1, 3, 13, Blue},
{"������'�", 3, 1, 2, 2, Green},
{"����'���", 4, 2, 0, 3, Red},
{"����� �������", 5, 1, 1, 4, Green},
{"������", 6, 1, 2, 0, Red},
{"�������", 7, 0, 2, 5, NoTech},
{"�����", 8, 1, 0, 6, NoTech},
{"�����", 9, 2, 1, 7, NoTech},
{"������", 10, 1, 2, 8, Blue},
{"����", 11, 3, 0, 9, NoTech},
{"��� �����", 12, 0, 2, 8, Red},
{"�������", 13, 2, 2, 10, NoTech},
{"�����", 14, 1, 1, 11, NoTech},
{"����� �����", 15, 1, 3, 12, Blue},
{"���'���", 16, 1, 1, 2, NoTech},
{"������", 17, 1, 2, 13, NoTech},
{"����� II", 18, 2, 0, 14, Red},
{"�����", 19, 3, 1, 15, Green, WormholeAlpha},
{"����", 20, 2, 1, 0, Green, WormholeBeta},
{"����� IV", 1, 2, 3, 2, NoTech},
{"����", 2, 1, 1, 3, NoTech},
{"�����", 3, 0, 3, 5, NoTech},
{"������", 4, 0, 3, 6, Blue},
{"���������", 5, 3, 1, 7, NoTech},
{"��������", 6, 2, 0, 0, NoTech},
{"������", 7, 3, 1, 14, Blue},
{"�������", 8, 2, 1, 2, NoTech},
{"���", 9, 1, 2, 3, Red},
{"���", 10, 0, 4, 6, NoTech},
{"�����", 11, 2, 0, 0, Blue},
{"������", 12, 1, 1, 2, Green},
};
struct solar_range {
	unit_type_s	type;
	char		from;
	char		to;
};
static solar_range solar_range_data[] = {{SolarSystem, 21, 24},
{AsteroidField, 25, 28},
{Nebula, 29, 30},
{Supernova, 31, 32},
};

int	planet_info::getinfluence() const {
	return influence;
}

int	planet_info::getresource() const {
	return resource;
}

unsigned planet_info::select(planet_info** result, planet_info* const*result_max, const char* home) {
	auto p = result;
	for(auto& e : planets) {
		if(!e.home)
			continue;
		if(strcmp(e.home, home) != 0)
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

unsigned planet_info::select(planet_info** result, planet_info* const* result_max, unit_info* parent) {
	auto p = result;
	for(auto& e : planets) {
		if(e.parent != parent)
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

int planet_info::get(const player_info* player, int(planet_info::*getproc)() const) {
	auto result = 0;
	for(auto& e : planets) {
		if(e.player != player)
			continue;
		result += (e.*getproc)();
	}
	return result;
}

void planet_info::refresh() {
	for(auto& e : planets)
		e.used = false;
}

void planet_info::initialize() {
	for(auto& e : solars)
		e.type = SolarSystem;
	for(auto& e : solar_range_data) {
		for(auto i = e.from; i <= e.to; i++)
			solars[i].type = e.type;
	}
	for(auto& e : planets) {
		e.type = Planet;
		e.player = 0;
		if(e.solar == -1)
			e.parent = 0;
		else
			e.parent = solars + e.solar;
	}
}

void planet_info::setup() {
	for(auto& e : planets) {
		if(!e.home)
			continue;
		e.player = player_info::find(e.home);
	}
}

int get_system_count() {
	auto result = 0;
	for(auto e : solar_map) {
		if(e == -1)
			continue;
		result++;
	}
	return result;
}

void planet_info::create_stars() {
	char player_pos[][2] = {{3, 0}, {6, 0}, {0, 3}, {6, 3}, {0, 6}, {3, 6}};
	memset(solar_map, 0, sizeof(solar_map));
	for(auto y = 0; y < 7; y++) {
		if(y < 3) {
			for(auto x = 0; x < (3 - y); x++)
				solar_map[gmi(x, y)] = -1;
		} else {
			for(auto x = 7 - (y-3); x < 8; x++)
				solar_map[gmi(x, y)] = -1;
		}
		solar_map[gmi(7, y)] = -1;
	}
	for(auto x = 0; x < 8; x++)
		solar_map[gmi(x, 7)] = -1;
	adat<int, 32> solar_deck;
	for(auto i = 1; i <= 32; i++)
		solar_deck.add(i);
	zshuffle(solar_deck.data, solar_deck.count);
	solar_map[gmi(3, 3)] = -1; // ������� ����
	for(auto e : player_pos)
		solar_map[gmi(e[0], e[1])] = -1;
	auto allowed_system = get_system_count();
	auto index = 0;
	for(auto& e : solar_map) {
		if(e == -1)
			continue;
		if(index<solar_deck.getcount())
			e = solar_deck[index++];
	}
	solar_map[gmi(3, 3)] = 0; // ����������� �������� ����
	// ���������� �������
	index = 33;
	int player_index = 0;
	for(auto& e : players) {
		for(auto& pn : planets) {
			if(!pn.home)
				continue;
			if(strcmp(pn.home, e.id) == 0)
				pn.parent = solars + index;
		}
		solar_map[gmi(player_pos[player_index][0], player_pos[player_index][1])] = index;
		player_index++;
		index++;
	}
}

bool unit_info::issolar() const {
	return this >= solars && this < (solars + sizeof(solars) / sizeof(solars[0]));
}

bool unit_info::isplanet() const {
	return this >= planets && this < (planets + sizeof(planets) / sizeof(planets[0]));
}

const char* unit_info::getsolarname() const {
	for(auto& e : planets) {
		if(!e)
			continue;
		if(e.parent == this)
			return e.name;
	}
	return getstr(type);
}

const char* unit_info::getplanetname() const {
	return (static_cast<const planet_info*>(this))->name;
}