#include "adat.h"
#include "aref.h"
#include "crt.h"
#include "cflags.h"

#pragma once

enum player_s : unsigned char {
	NoPlayer,
	TheXxchaKingdom,
	TheBaronyOfLetnev,
	TheNaaluCollective,
	TheL1z1xMindnet,
	TheYssarilTribes,
	FederationOfSol,
	TheMentakCoalition,
	TheEmiratesOfHacan,
	UniversitiesOfJolNar,
	SardakkNOrr,
	FirstPlayer = TheXxchaKingdom,
	LastPlayer = SardakkNOrr,
};
enum politic_s : unsigned char {
	NoPolitic,
	Initiative, Diplomacy, Political, Logistics,
	Trade, Warfare, Technology, Imperial,
};
enum tech_s : unsigned char {
	AdvancedFighters, AntimassDeflectors, FleetLogistics, LightWaveDeflector, TypeIVDrive, XRDTransporters,
	Cybernetics, DacxiveAnimators, GenSynthesis, NeuralMotivator, StasisCapsules, X89BacterialWeapon,
	AssaultCannon, DeepSpaceCannon, GravitonNegator, HylarVAssaultLaser, MagenDefenseGrid, WarSunTech,
	EnviroCompensator, GravitonLaserSystem, IntegratedEconomy, MicroTechnology, SarweenTools, TransitDiodes
};
enum tech_color_s : unsigned char {
	NoTech, Green, Blue, Red, Yellow,
};
enum wormhole_s : unsigned char {
	NoHole, WormholeAlpha, WormholeBeta
};
enum unit_s : unsigned char {
	NoUnit,
	SolarSystem, Nebula,
	Planet,
	SpaceDock,
	GroundForces, Fighters, PDS,
	Carrier, Cruiser, Destroyer, Dreadnought, WarSun,
};
enum command_s {
	NoCommand,
	AcceptButton, YesButton, NoButton, CancelButton
};
struct unit;
struct weapon {
	char				chance;
	char				count;
	char				bonus;
	char				reroll;
	constexpr weapon() : chance(0), count(1), bonus(0), reroll(0) {}
	constexpr weapon(char chance) : chance(chance), count(1), bonus(0), reroll(0) {}
	constexpr weapon(char chance, char count) : chance(chance), count(count), bonus(0), reroll(0) {}
	explicit operator bool() const { return chance != 0; }
	void				clear();
	int					roll() const;
};
struct player {
	char				command, strategy, fleet, goods;
	cflags<tech_s>		technologies;
	politic_s			politic;
	bool				ingame;
	bool				interactive;
	//
	unit*				create(unit_s id, unit* planet);
	void				initialize();
	bool				is(player_s value) const;
	int					getcommand() const { return command; }
	int					getfleet() const;
	player_s			getindex() const;
	int					getinitiative() const;
	static int			getinitiative(politic_s value);
	int					getstrategy() const { return strategy; }
	int					getgoods() const { return goods; }
};
extern player			players[SardakkNOrr + 1];
struct unit {
	unit_s				type;
	player_s			player;
	unit*				parent;
	bool				used;
	constexpr unit() : type(NoUnit), player(NoPlayer), parent(0), used(false) {}
	constexpr unit(unit_s type) : type(type), player(NoPlayer), parent(0), used(false) {}
	constexpr unit(unit_s type, unit* parent, player_s player) : type(type), player(player), parent(parent), used(false) {}
	explicit operator bool() const { return type != NoUnit; }
	void* operator new(unsigned size);
	void operator delete(void* pointer, unsigned size) {}
	~unit();
	bool				build(unit_s object, bool run);
	void				destroy();
	unit*				get(unit_s parent_type);
	static int			getavailable(unit_s type);
	int					getcapacity() const;
	unit_s				getcapacitylimit() const;
	int					getcarried() const;
	static int			getcount(unit_s type, player_s player, unit* location = 0);
	int					getfightersupport();
	virtual int			getinfluence() const { return 0; }
	int					getjoincount(unit_s object) const;
	int					getmaxhits() const;
	int					getmovement() const;
	virtual const char*	getname() const;
	static int			getproduction(unit_s type);
	int					getproduction() const { return getproduction(type); }
	virtual int			getresource() const;
	int					getstrenght() const { return getweapon().chance; }
	weapon				getweapon() const;
	weapon				getweapon(bool attacker, player_s opponent, char round) const;
	int					getweight() const;
	bool				is(player_s type) const { return players[player].is(type); }
	bool				iscarrier() const { return getcapacity() != 0; }
	bool				isinvaders() const;
	bool				isplanetary() const { return isplanetary(type); }
	static bool			isplanetary(unit_s type);
	bool				in(const unit* parent) const;
};
struct planet : unit {
	const char*			name;
	tech_color_s		tech_color;
	wormhole_s			wormhole;
	char				resource;
	char				influence;
	constexpr planet(const char* name, char solar, char resource, char influence,
		tech_color_s tech_color = NoTech,
		wormhole_s wormhole = NoHole);
	constexpr planet(const char* name, player_s player, char resource, char influence);
	static int			get(player_s player, int(planet::*getproc)() const);
	virtual const char*	getname() const override { return name; }
	virtual int			getinfluence() const override;
	int					getone() const { return 1; }
	virtual int			getresource() const override;
	static void			refresh();
};
extern unit				solars[38];
extern adat<unit, 256>	units;
struct army : adat<unit*, 32> {
	void				removecasualty(player_s player);
	void				sort(int (unit::*proc)() const);
};
struct querry : adat<unsigned char, 256> {
	querry() {}
	querry(unsigned char i1, unsigned char i2, bool(*select)(unsigned char index)) { add(i1, i2, select); }
	void				add(unsigned char i1, unsigned char i2, bool(*select)(unsigned char index));
	void				sort(int(*compare)(const void* p1, const void* p2));
	template<class T> aref<T> col() { return aref<T>((T*)data, count); }
};
namespace game {
unsigned char			choose(player_s player, unsigned char i1, unsigned char i2, const char* title, bool(*select)(unsigned char index), const char* (*source_getname)(unsigned char index), int(*compare)(const void* p1, const void* p2));
void					strategic();
};
unit*					getminimal(unit** result, unsigned count, int (unit::*get)() const);
unsigned				select(unit** result, unit** result_max, unit* location, player_s player, bool (unit::*test)() const = 0);
unsigned				select(planet** result, planet** result_max, unit* parent);