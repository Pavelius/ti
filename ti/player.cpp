#include "main.h"

struct player_statistic {
	const char*			name;
	unit_s				start_units[16];
	cflags<tech_s>		start_tech;
};
static constexpr player_statistic player_data[] = {{""},
{"The Xxcha Kingdom",
{Fighters, Fighters, Fighters, PDS, Carrier, GroundForces, GroundForces, Cruiser, Cruiser},
{AntimassDeflectors, EnviroCompensator}
},
{"The Barony of Letnev",
{Dreadnought, Destroyer, Carrier, GroundForces, GroundForces, GroundForces},
{HylarVAssaultLaser, AntimassDeflectors}
},
{"The Naalu Collective",
{GroundForces, GroundForces, GroundForces, GroundForces, PDS, Carrier, Cruiser, Destroyer, Fighters, Fighters, Fighters, Fighters},
{EnviroCompensator, AntimassDeflectors}
},
{"The L1z1x Mindnet",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Dreadnought, Fighters, Fighters, Fighters, PDS},
{EnviroCompensator, StasisCapsules, Cybernetics, HylarVAssaultLaser}
},
{"The Yssaril Tribes",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters, PDS},
{Cybernetics, AntimassDeflectors}
},
{"Federation of Sol",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Destroyer},
{AntimassDeflectors, XRDTransporters}
},
{"The Mentak Coalition",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, Cruiser, Cruiser, PDS},
{EnviroCompensator, HylarVAssaultLaser}
},
{"The Emirates of Hacan",
{GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Carrier, Cruiser, Fighters, Fighters},
{EnviroCompensator, SarweenTools}
},
{"Universities of Jol-Nar",
{GroundForces, GroundForces, Carrier, Carrier, Fighters, PDS, PDS, Dreadnought},
{AntimassDeflectors, HylarVAssaultLaser, EnviroCompensator, SarweenTools}
},
{"Sardakk N'orr",
{GroundForces, GroundForces, GroundForces, GroundForces, GroundForces, Carrier, Cruiser, PDS},
{DeepSpaceCannon, HylarVAssaultLaser}
},
};
getstr_enum(player);

player_info players[SardakkNOrr + 1];

static int compare_planets(const void* p1, const void* p2) {
	auto e1 = *((planet**)p1);
	auto e2 = *((planet**)p2);
	return e2->resource - e1->resource;
}

bool player_info::is(player_s value) const {
	return (this - players) == value;
}

int player_info::getfleet() const {
	auto result = fleet;
	if(is(TheBaronyOfLetnev))
		result++;
	return result;
}

unit* player_info::create(unit_s id, unit* planet) {
	return new unit(id, planet, getindex());
}

void player_info::initialize() {
	auto player = getindex();
	auto& pi = player_data[player];
	interactive = false;
	ingame = true;
	technologies.data = pi.start_tech.data;
	// Game setup: step 10
	auto solar_system = solars + player;
	adat<planet*, 8> planets;
	//planets.count = select(planets.data, planets.data + planets.getmaximum(), solar_system);
	//if(!planets.count)
	//	return;
	qsort(planets.data, planets.count, sizeof(planets.data[0]), compare_planets);
	auto base_planet = planets.data[0];
	create(SpaceDock, base_planet);
	for(auto e : pi.start_units) {
		if(!e)
			break;
		auto base = solar_system;
		switch(e) {
		case GroundForces:
		case PDS:
			base = base_planet;
			break;
		}
		create(e, base);
	}
	// Game setup: step 11
	fleet = 3;
	command = 3;
	strategy = 2;
}

player_s player_info::getindex() const {
	return (player_s)(this - players);
}

int	player_info::getinitiative() const {
	auto result = getinitiative(politic);
	if(is(TheNaaluCollective))
		result = 0;
	return result;
}

static unsigned select(player_info** source, unsigned maximum) {
	auto ps = source;
	auto pe = ps + maximum;
	for(auto& e : players) {
		if(!e)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - source;
}

static void strategic_phase() {
	adat<player_info*, 32> source;
	source.count = select(source.data, source.getmaximum());
	for(auto p : source)
		p->politic = NoPolitic;
	adat<politic_s, Imperial + 1> politics;
	for(auto i = Initiative; i <= Imperial; i = (politic_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		if(!p->iscomputer()) {
			answer_info ai;
			for(auto i : politics)
				ai.add(i, getstr(i));
			ai.sort();
			p->politic = (politic_s)ai.choose(false, "Эта стратегическая фаза. Вам нужно выбрать **одну** политику, которую будете использовать на этот ход. Будьте внимательны. Ваши враги также выбирают одну политику из этого же списка.");
		} else
			p->politic = politics.data[rand() % politics.getcount()];
		politics.remove(p->politic);
	}
}

void player_info::make_move() {
	strategic_phase();
}