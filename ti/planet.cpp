#include "main.h"

int			solar_map[8 * 8];
unit_info	solars[38];

static planet_info planets[] = {{"Archon Ren", "xxcha", 2, 3, 0},
{"Arc Prime", "barony", 4, 0, 0},
{"Maaluuk", "naalu", 0, 2, 0},
{"[0.0.0]", "mindnet", 5, 0, 0},
{"Retillion", "yssaril", 2, 3, 0},
{"Jord", "sol", 4, 2, 0},
{"Moll Primus", "mentak", 4, 1, 0},
{"Arretze", "hacan", 2, 0, 0},
{"Nar", "jelnar", 2, 1, 0},
{"Tren'Lak", "norr", 1, 0, 0},
//
{"Archon Tau", "xxcha", 1, 1, 1},
{"Wren Terra", "barony", 2, 1, 1},
{"Druaa", "naalu", 3, 1, 1},
{"Shalloq", "yssaril", 1, 2, 1},
{"Hercant", "hacan", 1, 1, 1},
{"Jol", "jelnar", 1, 2, 1},
{"Quinarra", "norr", 3, 1, 1},
//
{"Kamdorn", "hacan", 0, 1, 2},
//
{"Mecatol Rex", 0, 1, 6, 16, NoTech},
{"Bereg", 1, 3, 1, 12, Green},
{"Centauri", 2, 1, 3, 13, Blue},
{"Qucen'n", 3, 1, 2, 2, Green},
{"Tequ'ran", 4, 2, 0, 3, Red},
{"New Albion", 5, 1, 1, 4, Green},
{"Coorneeq", 6, 1, 2, 0, Red},
{"Mellon", 7, 0, 2, 5, NoTech},
{"Lazar", 8, 1, 0, 6, NoTech},
{"Arnor", 9, 2, 1, 7, NoTech},
{"Arinam", 10, 1, 2, 8, Blue},
{"Abyz", 11, 3, 0, 9, NoTech},
{"Dal Bootha", 12, 0, 2, 8, Red},
{"Saudor", 13, 2, 2, 10, NoTech},
{"Tibah", 14, 1, 1, 11, NoTech},
{"Mehar Xull", 15, 1, 3, 12, Blue},
{"Tar'Mann", 16, 1, 1, 2, NoTech},
{"Wellon", 17, 1, 2, 13, NoTech},
{"Vefut II", 18, 2, 0, 14, Red},
{"Lodor", 19, 3, 1, 15, Green, WormholeAlpha},
{"Quenn", 20, 2, 1, 0, Green, WormholeBeta},
{"Lirta IV", 1, 2, 3, 2, NoTech},
{"Gral", 2, 1, 1, 3, NoTech},
{"Rarron", 3, 0, 3, 5, NoTech},
{"Torkan", 4, 0, 3, 6, Blue},
{"Starpoint", 5, 3, 1, 7, NoTech},
{"Resculon", 6, 2, 0, 0, NoTech},
{"Zohbat", 7, 3, 1, 14, Blue},
{"Sakulag", 8, 2, 1, 2, NoTech},
{"Lor", 9, 1, 2, 3, Red},
{"Meer", 10, 0, 4, 6, NoTech},
{"Fira", 11, 2, 0, 0, Blue},
{"Xxehan", 12, 1, 1, 2, Green},
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
	memset(solar_map, 0, sizeof(solar_map));
	for(auto y = 0; y < 7; y++) {
		if(y < 3) {
			for(auto x = 0; x < (3 - y); x++)
				solar_map[gmi(x, y)] = -1;
			for(auto x = 7; x < 8; x++)
				solar_map[gmi(x, y)] = -1;
		} else {
			for(auto x = 7 - (y-3); x < 8; x++)
				solar_map[gmi(x, y)] = -1;
		}
	}
	for(auto x = 0; x < 8; x++)
		solar_map[gmi(x, 7)] = -1;
	int indecies[sizeof(solars)/ sizeof(solars[0])];
	int random_count = 0;
	while(random_count < 21) {
		indecies[random_count] = random_count;
		random_count++;
	}
	solar_map[gmi(3, 3)] = -1;
	auto allowed_system = get_system_count();
	while(allowed_system > random_count) {
		indecies[random_count] = random_count;
		auto d = d100();
		if(d < 50)
			solars[random_count].type = SolarSystem;
		else if(d<75)
			solars[random_count].type = AsteroidField;
		else if(d<90)
			solars[random_count].type = Nebula;
		else
			solars[random_count].type = Supernova;
		random_count++;
	}
	zshuffle(indecies+1, random_count-1);
	int index = 1;
	for(auto& e : solar_map) {
		if(e == -1)
			continue;
		e = indecies[index++];
	}
	solar_map[gmi(3, 3)] = indecies[0];
}