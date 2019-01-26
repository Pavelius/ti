#include "main.h"

unit_info solars[38];

constexpr planet_info::planet_info(const char* name, char solar, char resource, char influence, tech_color_s tech_color, wormhole_s wormhole) : unit_info(Planet),
name(name), home(0),
resource(resource), influence(influence),
tech_color(tech_color),
wormhole(wormhole) {}

constexpr planet_info::planet_info(const char* name, const char* home, char resource, char influence) : unit_info(Planet),
name(name), home(home),
resource(resource), influence(influence),
tech_color(NoTech),
wormhole(NoHole) {}

static planet_info planets[] = {{"Archon Ren", "xxcha", 2, 3},
{"Arc Prime", "barony", 4, 0},
{"Maaluuk", "naalu", 0, 2},
{"[0.0.0]", "mindnet", 5, 0},
{"Retillion", "yssaril", 2, 3},
{"Jord", "sol", 4, 2},
{"Moll Primus", "mentak", 4, 1},
{"Arretze", "hacan", 2, 0},
{"Nar", "jelnar", 2, 1},
{"Tren'Lak", "norr", 1, 0},
//
{"Archon Tau", "xxcha", 1, 1},
{"Wren Terra", "barony", 2, 1},
{"Druaa", "naalu", 3, 1},
{"Shalloq", "yssaril", 1, 2},
{"Hercant", "hacan", 1, 1},
{"Jol", "jelnar", 1, 2},
{"Quinarra", "norr", 3, 1},
//
{"Kamdorn", "hacan", 0, 1},
//
{"Mecatol Rex", 0, 1, 6, NoTech},
{"Bereg", 1, 3, 1, Green},
{"Centauri", 2, 1, 3, Blue},
{"Qucen'n", 3, 1, 2, Green},
{"Tequ'ran", 4, 2, 0, Red},
{"New Albion", 5, 1, 1, Green},
{"Coorneeq", 6, 1, 2, Red},
{"Mellon", 7, 0, 2, NoTech},
{"Lazar", 8, 1, 0, NoTech},
{"Arnor", 9, 2, 1, NoTech},
{"Arinam", 10, 1, 2, Blue},
{"Abyz", 11, 3, 0, NoTech},
{"Dal Bootha", 12, 0, 2, Red},
{"Saudor", 13, 2, 2, NoTech},
{"Tibah", 14, 1, 1, NoTech},
{"Mehar Xull", 15, 1, 3, Blue},
{"Tar'Mann", 16, 1, 1, NoTech},
{"Wellon", 17, 1, 2, NoTech},
{"Vefut II", 18, 2, 0, Red},
{"Lodor", 19, 3, 1, Green, WormholeAlpha},
{"Quenn", 20, 2, 1, Green, WormholeBeta},
{"Lirta IV", 1, 2, 3, NoTech},
{"Gral", 2, 1, 1, NoTech},
{"Rarron", 3, 0, 3, NoTech},
{"Torkan", 4, 0, 3, Blue},
{"Starpoint", 5, 3, 1, NoTech},
{"Resculon", 6, 2, 0, NoTech},
{"Zohbat", 7, 3, 1, Blue},
{"Sakulag", 8, 2, 1, NoTech},
{"Lor", 9, 1, 2, Red},
{"Meer", 10, 0, 4, NoTech},
{"Fira", 11, 2, 0, Blue},
{"Xxehan", 12, 1, 1, Green},
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
	for(auto& e : planets)
		e.player = 0;
}