#include "main.h"

unit solars[38];

constexpr planet::planet(const char* name, char solar, char resource, char influence, tech_color_s tech_color, wormhole_s wormhole) : unit(Planet, solars + solar + LastPlayer + 1, NoPlayer),
name(name),
resource(resource),
influence(influence),
tech_color(tech_color),
wormhole(wormhole) {}

constexpr planet::planet(const char* name, player_s player, char resource, char influence) : unit(Planet, solars + player, player),
name(name),
resource(resource),
influence(influence),
tech_color(NoTech),
wormhole(NoHole) {}

static planet planets[] = {{"Archon Ren", TheXxchaKingdom, 2, 3},
{"Arc Prime", TheBaronyOfLetnev, 4, 0},
{"Maaluuk", TheNaaluCollective, 0, 2},
{"[0.0.0]", TheL1z1xMindnet, 5, 0},
{"Retillion", TheYssarilTribes, 2, 3},
{"Jord", FederationOfSol, 4, 2},
{"Moll Primus", TheMentakCoalition, 4, 1},
{"Arretze", TheEmiratesOfHacan, 2, 0},
{"Nar", UniversitiesOfJolNar, 2, 1},
{"Tren'Lak", SardakkNOrr, 1, 0},
//
{"Archon Tau", TheXxchaKingdom, 1, 1},
{"Wren Terra", TheBaronyOfLetnev, 2, 1},
{"Druaa", TheNaaluCollective, 3, 1},
{"Shalloq", TheYssarilTribes, 1, 2},
{"Hercant", TheEmiratesOfHacan, 1, 1},
{"Jol", UniversitiesOfJolNar, 1, 2},
{"Quinarra", SardakkNOrr, 3, 1},
//
{"Kamdorn", TheEmiratesOfHacan, 0, 1},
//
{"Mecatol Rex", 0, 1, 6},
{"Bereg", 1, 3, 1, Green},
{"Centauri", 2, 1, 3, Blue},
{"Qucen'n", 3, 1, 2, Green},
{"Tequ'ran", 4, 2, 0, Red},
{"New Albion", 5, 1, 1, Green},
{"Coorneeq", 6, 1, 2, Red},
{"Mellon", 7, 0, 2},
{"Lazar", 8, 1, 0},
{"Arnor", 9, 2, 1},
{"Arinam", 10, 1, 2, Blue},
{"Abyz", 11, 3, 0},
{"Dal Bootha", 12, 0, 2, Red},
{"Saudor", 13, 2, 2},
{"Tibah", 14, 1, 1},
{"Mehar Xull", 15, 1, 3, Blue},
{"Tar'Mann", 16, 1, 1},
{"Wellon", 17, 1, 2},
{"Vefut II", 18, 2, 0, Red},
{"Lodor", 19, 3, 1, Green, WormholeAlpha},
{"Quenn", 20, 2, 1, Green, WormholeBeta},
{"Lirta IV", 1, 2, 3},
{"Gral", 2, 1, 1},
{"Rarron", 3, 0, 3},
{"Torkan", 4, 0, 3, Blue},
{"Starpoint", 5, 3, 1},
{"Resculon", 6, 2, 0},
{"Zohbat", 7, 3, 1, Blue},
{"Sakulag", 8, 2, 1},
{"Lor", 9, 1, 2, Red},
{"Meer", 10, 0, 4},
{"Fira", 11, 2, 0, Blue},
{"Xxehan", 12, 1, 1, Green},
};

unsigned select(planet** result, planet** result_max, unit* parent) {
	auto p = result;
	for(auto& e : planets) {
		if(e.parent != parent)
			continue;
		if(p < result_max)
			*p++ = &e;
	}
	return p - result;
}

void planet::refresh() {
	for(auto& e : planets)
		e.used = false;
}