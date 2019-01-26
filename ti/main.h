#include "bsreq.h"
#include "collection.h"
#include "crt.h"
#include "stringcreator.h"

#pragma once

bsreq player_type[];
bsreq string_type[];

enum play_s : unsigned char {
	NoPlay,
	AsAction, StrategicPhase,
	AfterByingTechnology,
	BeforeDrawPoliticCard, BeforeInvasion, BeforeSpaceCombat, BeforeCombatRound,
	BeforeStrategy, AfterHit,
};
enum strategy_s : unsigned char {
	NoStrategy,
	Initiative, Diplomacy, Political, Logistics,
	Trade, Warfare, Technology, Imperial,
};
enum action_s : unsigned char {
	NoAction,
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
	FirstActionCard = Armistice, LastActionCard = WarFooting,
	//
	BaronyEquipment, HacanTradeActionCards, JolanrRerollCombatDices, MentakAmbush, MentakPiracy,
	NaaluFleetRetreat, SolOrbitalDrop, ExecutePrimaryAbility, ChangePoliticCard, LookActionCards,
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
enum bonus_s : unsigned char {
	BonusActionCards, BonusCommandCounter, BonusInitiative, BonusFleetTokens, BonusTrade, BonusTechnology,
	BonusCostDreadnought,
	CombatBonusAll, CombatBonusDreadnought, CombatBonusGroundForcesAttack,
	CombatBonusDefend,
	CombatBonusFighters,
	CombatPenalty,
};
enum tech_color_s : unsigned char {
	NoTech, Green, Blue, Red, Yellow,
};
enum wormhole_s : unsigned char {
	NoHole, WormholeAlpha, WormholeBeta
};
enum unit_type_s : unsigned char {
	NoUnit,
	SolarSystem, Nebula,
	Planet,
	SpaceDock,
	GroundForces, Fighters, PDS,
	Carrier, Cruiser, Destroyer, Dreadnought, WarSun,
};
struct unit_info;
struct planet_info;
struct string;
struct name_info {
	const char*					id;
	const char*					name;
	const char*					getid() const { return id; }
	const char*					getname() const { return name; }
};
struct abstract_deck : adat<unsigned char> {
	void						clear();
	unsigned char				draw();
	void						discard(unsigned char);
	void						shuffle();
private:
	adat<unsigned char>			discarded;
};
template<class T>
struct deck : abstract_deck {
	void						add(T e) { auto p = abstract_deck::add(); *p = e; }
	T							draw() { return (T)abstract_deck::draw(); }
	void						discard(T e) { abstract_deck::discard(e); }
};
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
struct tactic_info {};
struct cost_info {
	void						add(action_s id, int v);
	void						difference(string& sb, const cost_info& e);
	int							get(action_s id) const { return actions[id]; }
	void						initialize();
	bool						is(action_s value) const { return actions[value] > 0; }
	void						set(action_s id, int v) { actions[id] = v; }
private:
	char						actions[LastAction + 1];
};
struct strategy_info {
	const char*					id;
	const char*					name;
	const char*					text;
	char						initiative;
	char						bonus;
};
struct player_info : name_info, cost_info {
	strategy_s					strategy;
	//
	explicit operator bool() const { return id != 0; }
	void						add(action_s id, int v) { cost_info::add(id, v); }
	void						add_action_cards(int value);
	void						add_command_tokens(int value);
	void						add_peace_pact(int value);
	void						add_trade_goods(int value) {}
	void						add_technology(int value) {}
	void						add_objective(int value) {}
	void						add_profit_for_trade_agreements() {}
	void						add_victory_points(int value) {}
	bool						build(adat<unit_info*> units, const planet_info* planet, const planet_info* system, int minimal, bool cancel_button);
	void						build_units(int value);
	void						cancel_all_trade_agreements() {}
	void						check_card_limin();
	player_info*				choose_opponent(const char* text);
	bool						choose_trade() const { return true; }
	void						create(const char* id);
	unit_info*					create(unit_type_s id, unit_info* planet);
	static void					create_action_deck();
	void						draw_political_card(int value) {}
	bool						is(action_s value) const { return cost_info::get(value); }
	bool						is(bonus_s value) const { return bonuses.is(value); }
	bool						is(tech_s value) const { return technologies.is(value); }
	bool						isallow(play_s type, action_s id) const;
	bool						isally(player_info* enemy) const;
	bool						iscomputer() const;
	bool						isenemy(player_info* enemy) const { return !isally(enemy); }
	static player_info*			find(const char* id);
	int							get(action_s id) const;
	int							getcardscount() const;
	static player_info*			gethuman();
	const char*					getid() const;
	void						getinfo(string& sb) const;
	int							getinitiative() const;
	static int					getinitiative(strategy_s value);
	const char*					getname() const;
	static player_info*			getspeaker();
	const char*					getyouname() const;
	static void					make_move();
	void						open_trade_negatiation() {}
	void						predict_next_political_card(int value) {}
	void						refresh_planets(int value) {}
	static action_s				report(const string& sb);
	void						return_command_from_board(int value) {}
	static void					setup();
	void						sethuman();
private:
	cflags<tech_s>				technologies;
	cflags<bonus_s>				bonuses;
};
typedef adat<player_info*, 6>	player_array;
struct unit_info {
	unit_type_s					type;
	player_info*				player;
	unit_info*					parent;
	bool						used;
	constexpr unit_info() : type(NoUnit), player(0), parent(0), used(false) {}
	constexpr unit_info(unit_type_s type) : type(type), player(0), parent(0), used(false) {}
	explicit operator bool() const { return type != NoUnit; }
	void* operator new(unsigned size);
	void operator delete(void* pointer, unsigned size) {}
	~unit_info();
	bool						build(unit_type_s object, bool run);
	void						destroy();
	unit_info*					get(unit_type_s parent_type);
	static int					getavailable(unit_type_s type);
	int							getcapacity() const;
	unit_type_s					getcapacitylimit() const;
	int							getcarried() const;
	int							getcount() const;
	static int					getcount(unit_type_s type, const player_info* player, unit_info* location = 0);
	int							getfightersupport();
	int							getjoincount(unit_type_s object) const;
	int							getmaxhits() const;
	int							getmovement() const;
	const char*					getname() const;
	static int					getproduction(unit_type_s type);
	int							getproduction() const { return getproduction(type); }
	int							getresource() const;
	int							getstrenght() const { return getweapon().chance; }
	weapon_info					getweapon() const;
	weapon_info					getweapon(bool attacker, const player_info* opponent, char round) const;
	int							getweight() const;
	bool						iscarrier() const { return getcapacity() != 0; }
	bool						isinvaders() const;
	bool						isplanetary() const { return isplanetary(type); }
	static bool					isplanetary(unit_type_s type);
	bool						in(const unit_info* parent) const;
};
struct planet_info : unit_info {
	const char*					name;
	const char*					home;
	tech_color_s				tech_color;
	wormhole_s					wormhole;
	char						resource;
	char						influence;
	constexpr planet_info(const char* name, char solar, char resource, char influence,
		tech_color_s tech_color,
		wormhole_s wormhole = NoHole);
	constexpr planet_info(const char* name, const char* home, char resource, char influence);
	static void					initialize();
	static int					get(const player_info* player, int(planet_info::*getproc)() const);
	const char*					getname() const { return name; }
	int							getinfluence() const;
	int							getone() const { return 1; }
	int							getresource() const;
	static void					refresh();
	static unsigned				select(planet_info** result, planet_info* const* result_max, unit_info* parent);
	static unsigned				select(planet_info** result, planet_info* const* result_max, const char* home);
};
struct army : adat<unit_info*, 32> {
	void						removecasualty(const player_info* player);
	void						sort(int (unit_info::*proc)() const);
};
struct string : stringcreator {
	const player_info*			player;
	string();
	void						addidentifier(const char* identifier) override;
private:
	char						buffer[8192];
};
struct answer_info : stringcreator {
	struct element {
		int						param;
		const char*				text;
		const char*				getname() const { return text; }
	};
	typedef void(*tips_proc)(stringcreator& sb, const element& e);
	adat<element, 8>			elements;
	constexpr explicit operator bool() const { return elements.count != 0; }
	answer_info();
	void						add(int param, const char* format, ...);
	void						addv(int param, const char* format, const char* format_param);
	int							choose(const char* format, const player_info* player) const;
	int							choosev(bool cancel_button, tips_proc tips, const char* picture, const char* format) const;
	void						sort();
private:
	char						buffer[4096];
};
extern deck<action_s>			action_deck;
extern player_info				players[6];
extern unit_info				solars[38];
extern strategy_info			strategy_data[];
extern adat<unit_info, 256>		units;