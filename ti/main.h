#include "collection.h"
#include "crt.h"
#include "stringcreator.h"

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
enum strategy_s : unsigned char {
	NoStrategy,
	Initiative, Diplomacy, Political, Logistics,
	Trade, Warfare, Technology, Imperial,
};
enum action_s : unsigned char {NoAction,
Armistice, ChemicalWarfare, CivilDefense, CommandSummit, CorporateSponsorship,
CouncilDissolved, CulturalCrisis, DeterminePolicy, DiplomaticImmunity, DirectHit,
Disclosure, Discredit, DugIn, EmergencyRepairs, ExperimentalBattlestation,
FantasticRhetoric, FighterPrototype, FlankSpeed, FocusedResearch, GhostShip,
GoodYear, GrandArmada, InTheSilenceOfSpace, InfluenceInTheMerchantsGuild, Insubordination,
IntoTheBreach, LocalUnrest, LuckyShot, MassiveTransport, MasterOfTrade,
Minelayers, MoraleBoost, Multiculturalism, OpeningTheBlackBox, Patrol,
Plague, PolicyParalysis, PoliticalStability, Privateers, ProductivitySpike,
PublicDisgrace, RallyOfThePeople, RareMineral, Recheck, Reparations,
RiseOfAMessiah, RuinousTariffs, Sabotage, ScientistAssassination, SecretIndustrialAgent,
ShieldsHolding, SignalJamming, SkilledRetreat, SpacedockAccident, StarOfDeath,
StellarCriminals, StrategicBombardment, StrategicFlexibility, StrategicShift, SuccessfulSpy,
Synchronicity, TechBubble, TouchOfGenius, TradeStop, Transport,
Thugs, UnexpectedAction, Uprising, Usurper, Veto,
VoluntaryAnnexation, WarFooting,
//
StrategyAction, TacticalAction, TransferAction, Pass,
Strategy, Fleet, Command, Goods,
LastAction = Goods,
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
struct unit;
struct string;
struct weapon_info {
	char						chance;
	char						count;
	char						bonus;
	char						reroll;
	constexpr weapon_info() : chance(0), count(1), bonus(0), reroll(0) {}
	constexpr weapon_info(char chance) : chance(chance), count(1), bonus(0), reroll(0) {}
	constexpr weapon_info(char chance, char count) : chance(chance), count(count), bonus(0), reroll(0) {}
	explicit operator bool() const { return chance != 0; }
	void						clear();
	int							roll() const;
};
struct tactic_info {
};
struct player_info {
	cflags<tech_s>				technologies;
	strategy_s					strategy;
	//
	static void					add(player_s id);
	void						add(action_s id, int v);
	void						add_action_cards(int value);
	void						add_command_tokens(int value);
	void						add_trade_goods(int value) {}
	void						add_technology(int value) {}
	void						add_objective(int value) {}
	void						add_profit_for_trade_agreements() {}
	void						add_victory_points(int value) {}
	void						cancel_all_trade_agreements() {}
	player_info*				choose_opponent(string& sb);
	bool						choose_trade() const { return true; }
	unit*						create(unit_s id, unit* planet);
	void						draw_political_card(int value) {}
	void						initialize();
	bool						is(player_s value) const;
	bool						is(action_s value) const { return actions[value] > 0; }
	bool						isally(player_info* enemy) const;
	bool						iscomputer() const;
	bool						isenemy(player_info* enemy) const { return !isally(enemy); }
	int							get(action_s id) const;
	static player_info*			gethuman();
	const char*					getid() const;
	player_s					getindex() const;
	void						getinfo(string& sb) const;
	int							getinitiative() const;
	static int					getinitiative(strategy_s value);
	const char*					getname() const;
	static player_info*			getspeaker();
	const char*					getyouname() const;
	static void					make_move();
	void						open_trade_negatiation() {}
	void						predict_next_political_card(int value) {}
	static action_s				report(const string& sb);
	void						return_command_from_board(int value) {}
	void						set(action_s id, int v) { actions[id] = v; }
	static void					setup();
	static void					sethuman(player_s id);
private:
	char						actions[LastAction + 1];
};
extern player_info				players[SardakkNOrr + 1];
typedef adat<player_info*, 6>	player_array;
struct unit {
	unit_s						type;
	player_s					player;
	unit*						parent;
	bool						used;
	constexpr unit() : type(NoUnit), player(NoPlayer), parent(0), used(false) {}
	constexpr unit(unit_s type) : type(type), player(NoPlayer), parent(0), used(false) {}
	constexpr unit(unit_s type, unit* parent, player_s player) : type(type), player(player), parent(parent), used(false) {}
	explicit operator bool() const { return type != NoUnit; }
	void* operator new(unsigned size);
	void operator delete(void* pointer, unsigned size) {}
	~unit();
	bool						build(unit_s object, bool run);
	void						destroy();
	unit*						get(unit_s parent_type);
	static int					getavailable(unit_s type);
	int							getcapacity() const;
	unit_s						getcapacitylimit() const;
	int							getcarried() const;
	static int					getcount(unit_s type, player_s player, unit* location = 0);
	int							getfightersupport();
	virtual int					getinfluence() const { return 0; }
	int							getjoincount(unit_s object) const;
	int							getmaxhits() const;
	int							getmovement() const;
	virtual const char*			getname() const;
	static int					getproduction(unit_s type);
	int							getproduction() const { return getproduction(type); }
	virtual int					getresource() const;
	int							getstrenght() const { return getweapon().chance; }
	weapon_info						getweapon() const;
	weapon_info						getweapon(bool attacker, player_s opponent, char round) const;
	int							getweight() const;
	bool						is(player_s type) const { return players[player].is(type); }
	bool						iscarrier() const { return getcapacity() != 0; }
	bool						isinvaders() const;
	bool						isplanetary() const { return isplanetary(type); }
	static bool					isplanetary(unit_s type);
	bool						in(const unit* parent) const;
};
struct planet_info : unit {
	const char*					name;
	tech_color_s				tech_color;
	wormhole_s					wormhole;
	char						resource;
	char						influence;
	constexpr planet_info(const char* name, char solar, char resource, char influence,
		tech_color_s tech_color = NoTech,
		wormhole_s wormhole = NoHole);
	constexpr planet_info(const char* name, player_s player, char resource, char influence);
	static int					get(player_s player, int(planet_info::*getproc)() const);
	virtual const char*			getname() const override { return name; }
	virtual int					getinfluence() const override;
	int							getone() const { return 1; }
	virtual int					getresource() const override;
	static void					refresh();
};
struct army : adat<unit*, 32> {
	void						removecasualty(player_s player);
	void						sort(int (unit::*proc)() const);
};
struct string : stringcreator {
	string();
	void						addidentifier(const char* identifier) override;
private:
	const struct army*			army;
	char						buffer[8192];
};
struct answer_info : string {
	struct element {
		int						param;
		const char*				text;
		const char*				getname() const { return text; }
	};
	typedef void(*tips_proc)(stringcreator& sb, const element& e);
	adat<element, 8>			elements;
	constexpr explicit operator bool() const { return elements.count != 0; }
	void						add(int param, const char* format, ...);
	void						addv(int param, const char* format, const char* format_param);
	int							choose(const char* format, bool random = false) const;
	int							choosev(bool cancel_button, tips_proc tips, const char* picture, const char* format) const;
	void						sort();
};
extern player_array				active_players;
extern unit						solars[38];
extern adat<unit, 256>			units;