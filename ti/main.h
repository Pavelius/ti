#include "adat.h"
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
	LastPlayer = SardakkNOrr,
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
	bool				ingame;
	bool				interactive;
	//
	void				initialize();
	player_s			getindex() const;
};
struct unit {
	unit_s				type;
	player_s			player;
	unit*				parent;
	bool				used;
	constexpr unit() : type(NoUnit), player(NoPlayer), parent(0), used(false) {}
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
	int					getjoincount(unit_s object) const;
	int					getmaxhits() const;
	int					getmovement() const;
	virtual const char*	getname() const;
	static int			getproduction(unit_s type);
	int					getproduction() const { return getproduction(type); }
	virtual int			getresource() const;
	weapon				getweapon() const;
	weapon				getweapon(bool attacker, player_s opponent, char round) const;
	int					getweight() const;
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
	virtual const char*	getname() const override { return name; }
	static void			refresh();
};
struct army : adat<unit*, 32> {
	void				removecasualty(player_s player);
	void				sort(int (unit::*proc)() const);
};
unit*					getminimal(unit** result, unsigned count, int (unit::*get)() const);
extern player			players[SardakkNOrr + 1];
unsigned				select(unit** result, unit** result_max, unit* location, player_s player, bool (unit::*test)() const = 0);
unsigned				select(planet** result, planet** result_max, unit* parent);
extern unit				solars[38];
extern adat<unit, 256>	units;