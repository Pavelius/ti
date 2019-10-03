#include "crt.h"
#include "stringbuilder.h"

#pragma once

const int map_scan_line = 8;
const unsigned char Blocked = 0xFF;
const unsigned char DefaultCost = Blocked - 1;

enum play_s : unsigned char {
	NoPlay,
	AsAction, StrategicPhase,
	AfterByingTechnology,
	BeforeDrawPoliticCard, BeforeInvasion, BeforeSpaceCombat, BeforeCombatRound,
	BeforeStrategy, AfterHit,
};
enum strategy_s : unsigned char {
	NoStrategy,
	Leadership, Diplomacy, Politics, Construction,
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
	StrategyAction, TacticalAction, Pass,
	Strategy, Fleet, Command,
	Commodities, Goods,
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
enum target_s : unsigned {
	TargetMask = 0xFF,
	Neutral = 0x100, Friendly = 0x200, DockPresent = 0x400, NoMekatol = 0x800,
	NoHome = 0x1000, FirstEntry = 0x2000,
};
enum variant_s : unsigned char {
	NoVariant,
	Player, Solar, AsteroidField, Nebula, Supernova,
	Planet, Unit, SpaceDock,
	GroundForces, Fighters, PDS,
	Carrier, Cruiser, Destroyer, Dreadnought, WarSun,
	Agenda, TechnologyVar,
	Variant
};
enum planet_s : unsigned char {
	PlanetUsed, Exhaused,
};
enum relation_s : unsigned char {
	WasHostile,
};
struct agendai;
class answeri;
class planeti;
class playeri;
class string;
class solari;
class uniti;
typedef adat<playeri*, 6> playera;
typedef adat<solari*, 64> solara;
typedef adat<planeti*, 64> planeta;
struct variant {
	variant_s					type;
	union {
		variant_s				var;
		tech_s					tech;
		unsigned char			value;
	};
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(variant_s t, decltype(value) v) : type(t), value(v) {}
	constexpr variant(tech_s v) : type(TechnologyVar), tech(v) {}
	constexpr variant(variant_s v) : type(Variant), var(v) {}
	template<class T> constexpr variant(variant_s t, const T* p) : type(p ? t : NoVariant), value(p ? p - bsmeta<T>::elements : 0) {}
	constexpr variant(const agendai* v) : variant(Agenda, v) {}
	constexpr variant(const planeti* v) : variant(Planet, v) {}
	constexpr variant(const playeri* v) : variant(Player, v) {}
	constexpr variant(const solari* v) : variant(Solar, v) {}
	constexpr variant(const uniti* v) : variant(Unit, v) {}
	constexpr bool operator==(const variant& e) const { return type == e.type && value == e.value; }
	constexpr operator bool() const { return type != NoVariant; }
	void						clear() { type = NoVariant; value = 0; }
	template<class T> constexpr T* get() const { return &bsmeta<T>::elements[value]; }
	constexpr agendai*			getagenda() const { return get<agendai>(); }
	constexpr planeti*			getplanet() const { return get<planeti>(); }
	constexpr playeri*			getplayer() const { return get<playeri>(); }
	constexpr solari*			getsolar() const { return get<solari>(); }
	constexpr uniti*			getunit() const { return get<uniti>(); }
};
typedef adat<variant, 64>		varianta;
struct army : adat<uniti*, 32> {
	void						removecasualty(const playeri* player);
	void						rollup();
	void						sort(int (uniti::*proc)() const);
};
struct namei {
	const char*					id;
	const char*					name;
	const char*					getid() const { return id; }
	const char*					getname() const { return name; }
};
class abstract_deck : public adat<unsigned char, 256> {
	adat<unsigned char>			discarded;
public:
	void						clear();
	unsigned char				draw();
	void						discard(unsigned char);
	void						shuffle();
	void						top(unsigned char);
};
class relation {
	char						reaction;
	cflags<relation_s, unsigned char> flags;
public:
	void						add(int v) { reaction += v; }
	int							get() const;
	bool						is(relation_s v) const { return flags.is(v); }
	void						set(relation_s v) { return flags.add(v); }
	void						remove(relation_s v) { return flags.remove(v); }
};
template<class T>
struct deck : abstract_deck {
	void						add(T e) { auto p = abstract_deck::add(); *p = e; }
	T							draw() { return (T)abstract_deck::draw(); }
	void						discard(T e) { abstract_deck::discard(e); }
};
struct weaponi {
	char						chance;
	char						count;
	char						bonus;
	char						reroll;
	constexpr weaponi() : chance(0), count(1), bonus(0), reroll(0) {}
	constexpr weaponi(char chance) : chance(chance), count(1), bonus(0), reroll(0) {}
	constexpr weaponi(char chance, char count) : chance(chance), count(count), bonus(0), reroll(0) {}
	explicit operator bool() const { return chance != 0; }
	void						clear();
	int							roll() const;
};
struct varianti {
	const char*					id;
	const char*					name;
	char						available;
	char						cost;
	char						production;
	char						movements;
	char						count;
	char						production_count;
	weaponi						combat;
};
class costi {
	char						actions[LastAction + 1];
public:
	void						add(action_s id, int v);
	void						difference(string& sb, const costi& e);
	int							get(action_s id) const { return actions[id]; }
	bool						is(action_s value) const { return actions[value] > 0; }
	void						set(action_s id, int v) { actions[id] = v; }
};
struct strategyi {
	const char*					id;
	const char*					name;
	char						initiative;
	const char*					text;
	char						bonus;
};
class playeri : public namei, public costi {
	relation					relations[8];
	char						commodities;
	cflags<tech_s>				technologies;
	cflags<bonus_s>				bonuses;
public:
	strategy_s					strategy;
	//
	explicit operator bool() const { return id != 0; }
	void						activate();
	void						add(action_s id, int v) { costi::add(id, v); }
	void						add_action_cards(int value);
	void						add_command_tokens(int value);
	void						add_technology(int value) {}
	void						add_trade_goods(int value);
	void						add_objective(int value) {}
	void						add_victory_points(int value) {}
	void						apply(string& sb);
	bool						build(army& units, const planeti* planet, solari* system, int resources, int fleet, int minimal, int maximal, bool cancel_button);
	void						build_units(int value);
	void						buy_command_tokens(int cost_influences);
	void						buy_technology(int cost_resources);
	void						choose_speaker(int exclude);
	void						check_card_limin();
	uniti*						choose(army& source, const char* format) const;
	int							choose(string& sb, answeri& ai, bool cancel_button, const char* format, ...) const;
	bool						choose(army& a1, army& a2, const char* action, bool cancel_button, bool show_movement = false) const;
	solari*						choose(const aref<solari*>& source) const;
	planeti*					choose(const aref<planeti*>& source, const char* format, ...) const;
	int							choosev(string& sb, answeri& ai, bool cancel_button, const char* format, const char* format_param) const;
	void						choose_diplomacy();
	bool						choose_movement(uniti* solar) const;
	playeri*					choose_opponent(const char* text);
	bool						choose_trade() const { return true; }
	playeri&					create(const char* id);
	uniti*						create(variant_s id, solari* solar);
	uniti*						create(variant_s id, planeti* planet);
	static void					create_action_deck();
	static void					create_agenda_deck();
	void						draw_political_card(int value) {}
	bool						is(action_s value) const { return costi::get(value); }
	bool						is(bonus_s value) const { return bonuses.is(value); }
	bool						is(tech_s value) const { return technologies.is(value); }
	bool						isactive() const { return getactive() == this; }
	bool						isallow(play_s type, action_s id) const;
	bool						isallow(variant_s id) const;
	bool						isally(const playeri* enemy) const;
	bool						iscomputer() const;
	bool						isenemy(const playeri* enemy) const { return !isally(enemy); }
	static playeri*				find(const char* id);
	int							get(action_s id) const;
	int							getreaction(const playeri* p) const { return relations[p->getid()].get(); }
	static playeri*				getactive();
	int							getcardscount() const;
	int							getcommodities() const { return commodities; }
	int							getfleet() const;
	static playeri*				gethuman();
	unsigned char				getid() const;
	void						getinfo(string& sb) const;
	int							getinitiative() const;
	static int					getinitiative(strategy_s value);
	int							getinfluences() const;
	solari*						gethomesystem() const;
	const char*					getname() const;
	int							getresources() const;
	static playeri*				getspeaker();
	const char*					getyouname() const;
	static void					make_move(bool strategic, bool action);
	void						message(const char* text);
	void						moveships(solari* solar);
	void						open_trade_negatiation() {}
	void						pay(int cost);
	void						predict_next_political_card(int value);
	void						refresh_planets(int value) {}
	void						replenish_commodities();
	void						return_command_from_board(int value) {}
	static void					slide(int x, int y);
	static void					slide(unsigned char hexagon);
	void						select(army& source, unsigned flags) const;
	void						select(solara& result, unsigned flags) const;
	void						select(planeta& result, unsigned flags) const;
	unsigned					select(uniti** result, uniti* const* result_maximum, unsigned flags, variant_s type) const;
	void						set(action_s id, char v) { costi::set(id, v); }
	static void					setup();
	void						sethuman();
	void						tactical_action();
};
class uniti {
	unsigned char				player;
	variant						parent;
	unsigned char				flags;
public:
	variant_s					type;
	constexpr uniti() : type(NoVariant), player(0xFF), parent(), flags(0) {}
	constexpr uniti(variant_s type) : type(type), player(0xFF), parent(), flags(0) {}
	constexpr uniti(variant_s type, variant parent) : type(type), player(0xFF), parent(), flags(0) {}
	explicit operator bool() const { return type != NoVariant; }
	void* operator new(unsigned size);
	void operator delete(void* pointer, unsigned size) {}
	~uniti();
	bool						build(variant_s object, bool run);
	void						destroy();
	int							getcapacity() const;
	variant_s					getcapacitylimit() const;
	int							getcarried() const;
	int							getcount() const;
	const varianti&				getgroup() const { return bsmeta<varianti>::elements[type]; }
	int							getjoincount(variant_s object) const;
	int							getmaxhits() const;
	int							getmovement() const;
	static int					getmovement(short unsigned index);
	const char*					getname() const { return getgroup().name; }
	playeri*					getplayer() const;
	static int					getproduction(variant_s type);
	int							getproduction() const { return getproduction(type); }
	int							getresource() const;
	planeti*					getplanet() const;
	int							getproduce() const;
	static int					getproduce(variant_s type);
	solari*						getsolar() const;
	int							getstrenght() const { return getweapon().chance; }
	weaponi						getweapon() const;
	weaponi						getweapon(bool attacker, const playeri* opponent, char round) const;
	int							getweight() const;
	bool						is(bonus_s v) const;
	bool						is(tech_s v) const;
	bool						iscarrier() const { return getcapacity() != 0; }
	bool						isfleet() const;
	bool						isinvaders() const;
	bool						isplanetary() const { return isplanetary(type); }
	static bool					isplanetary(variant_s type);
	void						setplanet(const planeti* v);
	void						setplayer(const playeri* v);
	void						setsolar(const solari* v);
};
class solari {
	variant_s					type;
	unsigned char				player;
	unsigned char				flags;
	unsigned char				index;
public:
	constexpr explicit operator bool() const { return index != 0xFF; }
	void						activate(const playeri* v);
	void						deactivate(const playeri* v) { flags &= ~(1 << (v->getid())); }
	int							getcount(variant_s type, const playeri* player) const;
	int							getfleet(const playeri* player) const;
	int							getfleetsupport(const playeri* player) const;
	variant_s					getgroup() const { return type; }
	unsigned char				getindex() const { return index; }
	const char*					getname() const;
	planeti*					getplanet(int index) const;
	playeri*					getplayer() const { return (player == 0xFF) ? 0 : &bsmeta<playeri>::elements[player]; }
	static solari*				getsolar(short unsigned index);
	bool						isactivated(const playeri* v) const { return (flags & (1 << v->getid())) != 0; }
	bool						ismekatol() const;
	//static void					select(solara& result, const playeri* player, bool include_mekatol_rex);
	void						select(planeta& result, unsigned flags) const;
	void						set(variant_s v) { type = v; }
	void						setindex(unsigned char v) { index = v; }
	void						setplayer(const playeri* v);
};
class planeti {
	const char*					name;
	const char*					home;
	unsigned char				solar;
	unsigned char				player;
	tech_color_s				tech_color;
	wormhole_s					wormhole;
	char						resource;
	char						influence;
	unsigned char				avatar;
	unsigned char				flags;
public:
	constexpr planeti(const char* name, unsigned char solar, char resource, char influence, char avatar, tech_color_s tech_color, wormhole_s wormhole = NoHole)
		: name(name), home(0), solar(solar), player(0xFF), resource(resource),
		influence(influence), avatar(avatar), tech_color(tech_color), wormhole(wormhole), flags(0) {
	}
	constexpr planeti(const char* name, const char* home, char resource, char influence, char avatar)
		: name(name), home(home), solar(0xFF), player(0xFF), resource(resource),
		influence(influence), avatar(avatar), tech_color(NoTech), wormhole(NoHole), flags(0) {
	}
	constexpr explicit operator bool() const { return flags != 0; }
	static void					create_stars();
	static void					initialize();
	static planeti*				find(const solari* parent, int index);
	static int					get(const playeri* player, int(planeti::*getproc)() const);
	unsigned char				getavatar() const { return avatar; }
	int							getcount(variant_s type, const playeri* player) const;
	const char*					gethome() const { return home; }
	const char*					getname() const { return name; }
	int							getinfluence() const;
	int							getone() const { return 1; }
	int							getresource() const;
	playeri*					getplayer() const { return (player == 0xFF) ? 0 : &bsmeta<playeri>::elements[player]; }
	solari*						getsolar() const { return &bsmeta<solari>::elements[solar]; }
	bool						is(planet_s v) const { return (flags & (1 << v)) != 0; }
	static void					refresh();
	void						remove(planet_s v) { flags &= ~(1 << v); }
	static void					setup();
	void						set(planet_s v) { flags |= 1 << v; }
	void						setplayer(const playeri* v) { if(v) player = v - bsmeta<playeri>::elements; else player = 0xFF; }
	void						setsolar(const solari* v) { if(v) solar = v - bsmeta<solari>::elements; else solar = 0xFF; }
};
struct actioni {
	typedef bool(*testp)(const playeri& player);
	struct proc_info {
		play_s		type;
		testp		test;
		constexpr proc_info() : type(NoPlay), test(0) {}
		constexpr proc_info(play_s type) : type(type), test(0) {}
		constexpr proc_info(testp proc) : type(AsAction), test(proc) {}
	};
	const char*					id;
	const char*					name;
	int							count;
	proc_info					proc;
	const char*					description;
};
struct agendai {
	typedef bool(*testp)(variant v);
	typedef void(*effectp)(variant v);
	const char*					id;
	const char*					name;
	unsigned					target;
	effectp						success, fail;
	unsigned char				count;
};
class string : public stringbuilder {
	char						buffer[8192];
public:
	const playeri*				player;
	string();
	void						addidentifier(const char* identifier) override;
};
class answeri : public stringbuilder {
	struct element {
		int						param;
		const char*				text;
		const char*				getname() const { return text; }
	};
	char						buffer[4096];
	adat<element, 8>			elements;
	typedef void(*tips_proc)(stringbuilder& sb, int param);
public:
	constexpr explicit operator bool() const { return elements.count != 0; }
	answeri();
	void						add(int param, const char* format, ...);
	void						addv(int param, const char* format, const char* format_param);
	int							choose(bool cancel_button, bool random_choose, tips_proc tips, const char* picture, const char* format) const;
	static int					compare(const void* p1, const void* p2);
	void						sort();
};
extern deck<action_s>			action_deck;
inline unsigned char			gmi(int x, int y) { return y * map_scan_line + x; }
inline char						gmx(unsigned char index) { return index % map_scan_line; }
inline char						gmy(unsigned char index) { return index / map_scan_line; }
DECLENUM(action);
DECLENUM(variant);
DECLENUM(strategy);