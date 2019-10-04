#include "crt.h"
#include "stringbuilder.h"

#pragma once

const int map_scan_line = 8;
const unsigned char Blocked = 0xFF;
const unsigned char DefaultCost = Blocked - 1;

enum play_s : unsigned char {
	NoPlay,
	AsAction,
	BeforeStrategy, BeforeAgendaPhase, BeforeBombardment, BeforeInvasion, BeforeSpaceCombat, BeforeCombatRound,
	AfterAgendaRevealed, AfterSpeakerVote, AfterByingTechnology, AfterDamage, AfterSustainDamage,
};
enum strategy_s : unsigned char {
	NoStrategy,
	Leadership, Diplomacy, Politics, Construction,
	Trade, Warfare, Technology, Imperial,
};
enum action_s : unsigned char {
	NoAction,
	AncientBurialSites, AssassinateRepresentative, Bunker, CrippleDefenses, DirectHit, Sabotage, Spy,
	Uprising, WarfareRider,
	FirstActionCard = AncientBurialSites, LastActionCard = WarfareRider,
	//
	StrategyAction, TacticalAction, Pass,
	Strategic, Fleet, Tactical,
	VictoryPoints,
	Commodities, Goods,
	LastAction = Goods,
	Resource, Influence,
};
enum tech_s : unsigned char {
	// Common technoogies
	PlasmaScoring, MagenDefenseGrid, DuraniumArmor, AssaultCannon,
	NeuralMotivator, DacxiveAnimators, HyperMetabolism, X89BacterialWeapon,
	AntimassDeflectors, GravityDriver, FleetLogistics, LightWaveDeflector,
	SarweenTools, GravitonLaserSystem, TransitDiodes, IntegratedEconomy,
	// War technologies
	CruiserII, DreadnoughtII, DestroyerII, PDSII, CarrierII, FighterII, InfantryII, SpaceDockII,
	WarSunTech,
	FirstTech = PlasmaScoring, LastTech = LightWaveDeflector,
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
	NoTech, Red, Green, Blue, Yellow,
};
enum wormhole_s : unsigned char {
	NoHole, WormholeAlpha, WormholeBeta
};
enum target_s : unsigned {
	TargetMask = 0xFF,
	Neutral = 0x100, Friendly = 0x200, DockPresent = 0x400, NoMekatol = 0x800,
	NoHome = 0x1000, FirstEntry = 0x2000, Activated = 0x4000, Ready = 0x8000,
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
enum object_s : unsigned char {
	ObjectUsed, Exhaused,
};
enum relation_s : unsigned char {
	WasHostile,
};
enum secret_s : unsigned char {
	UnveilFlagship, TurnTheirFleetsToDust, DestroyTheirGreatestShip, SparkARebellion, CutSupplyLines, MonopolizeProduction,
	FirstSecret = UnveilFlagship, LastSecret = MonopolizeProduction,
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
struct unita : adat<uniti*, 32> {
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
struct objectivei {
	const char*					id;
	const char*					name;
	char						vp;
};
class playeri : public namei, public costi {
	relation					relations[8];
	char						commodities;
	cflags<tech_s>				technologies;
	cflags<bonus_s>				bonuses;
	cflags<secret_s>			secrets;
public:
	strategy_s					strategy;
	//
	explicit operator bool() const { return id != 0; }
	void						activate();
	void						add(action_s id, int v) { costi::add(id, v); }
	void						add_action_cards(int value);
	void						add_command_tokens(int value);
	void						add_technology(int value);
	void						add_trade_goods(int value);
	void						add_objective(int value) {}
	void						add_secret_objective(int value, bool interactive = true);
	void						add_victory_points(int value);
	void						apply(const char* format);
	bool						build(unita& units, const planeti* planet, solari* system, int resources, int fleet, int minimal, int maximal, bool cancel_button);
	void						build_units(int value);
	void						buy_command_tokens(int cost_influences);
	void						buy_technology(int cost_resources);
	void						choose_speaker(int exclude);
	void						check_card_limin();
	uniti*						choose(unita& source, const char* format) const;
	bool						choose(unita& a1, unita& a2, const char* action, bool cancel_button, bool show_movement = false) const;
	solari*						choose(const aref<solari*>& source, const char* format) const;
	planeti*					choose(const aref<planeti*>& source, const char* format) const;
	int							choose(answeri& ai, bool cancel_button, const char* format) const;
	void						choose_diplomacy();
	bool						choose_movement(uniti* solar) const;
	playeri*					choose_opponent(const char* text);
	bool						choose_trade() const { return true; }
	playeri&					create(const char* id);
	uniti*						create(variant_s id, solari* solar);
	uniti*						create(variant_s id, planeti* planet);
	static void					create_action_deck();
	static void					create_agenda_deck();
	static void					create_objectve_deck();
	void						draw_political_card(int value) {}
	bool						is(action_s v) const { return costi::get(v); }
	bool						is(bonus_s v) const { return bonuses.is(v); }
	bool						is(tech_s v) const { return technologies.is(v); }
	bool						is(secret_s v) const { return secrets.is(v); }
	bool						isactive() const { return getactive() == this; }
	bool						isallow(play_s type, action_s id) const;
	bool						isallow(variant_s id) const;
	bool						isallow(tech_s v) const;
	bool						isally(const playeri* enemy) const;
	bool						iscomputer() const;
	bool						isenemy(const playeri* enemy) const { return !isally(enemy); }
	bool						ispassed() const { return get(Pass) == 0; }
	static playeri*				find(const char* id);
	int							get(action_s id) const;
	static playeri*				get(const char* id);
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
	int							pay(int maximum, int cost, const char* subject, const char* subjects, action_s currency);
	void						pay_apply(int cost, action_s currency);
	int							pay_choose(int maximum, int cost, const char* subject, const char* subjects, action_s currency) const;
	void						predict_next_political_card(int value);
	void						replenish_commodities();
	void						remove(secret_s v) { secrets.remove(v); }
	static void					slide(int x, int y);
	static void					slide(unsigned char hexagon);
	void						select(unita& source, unsigned flags) const;
	void						select(solara& result, unsigned flags) const;
	void						select(planeta& result, unsigned flags) const;
	unsigned					select(uniti** result, uniti* const* result_maximum, unsigned flags, variant_s type) const;
	void						selectp(solara& result, unsigned flags) const;
	void						set(action_s id, char v) { costi::set(id, v); }
	void						set(tech_s v) { technologies.add(v); }
	void						set(secret_s v) { secrets.add(v); }
	static void					setup();
	void						sethuman();
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
	uniti*						find(variant_s group) const;
	int							getcount(variant_s type, const playeri* player) const;
	int							getfleet(const playeri* player) const;
	int							getfleetsupport(const playeri* player) const;
	variant_s					getgroup() const { return type; }
	unsigned char				getindex() const { return index; }
	static solari*				getmekatol();
	const char*					getname() const;
	planeti*					getplanet(int index) const;
	playeri*					getplayer() const { return (player == 0xFF) ? 0 : &bsmeta<playeri>::elements[player]; }
	static solari*				getsolar(short unsigned index);
	bool						isactivated(const playeri* v) const { return (flags & (1 << v->getid())) != 0; }
	bool						ismekatol() const;
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
	static int					get(const playeri* player, int(planeti::*getproc)() const, unsigned flags = 0);
	int							get(action_s id) const;
	unsigned char				getavatar() const { return avatar; }
	int							getcount(variant_s type, const playeri* player) const;
	const char*					gethome() const { return home; }
	const char*					getname() const { return name; }
	int							getinfluence() const;
	int							getone() const { return 1; }
	int							getproduction() const;
	int							getresource() const;
	playeri*					getplayer() const { return (player == 0xFF) ? 0 : &bsmeta<playeri>::elements[player]; }
	solari*						getsolar() const { return &bsmeta<solari>::elements[solar]; }
	bool						is(object_s v) const { return (flags & (1 << v)) != 0; }
	static void					refresh();
	void						remove(object_s v) { flags &= ~(1 << v); }
	static void					setup();
	void						set(object_s v) { flags |= 1 << v; }
	void						setplayer(const playeri* v) { if(v) player = v - bsmeta<playeri>::elements; else player = 0xFF; }
	void						setsolar(const solari* v) { if(v) solar = v - bsmeta<solari>::elements; else solar = 0xFF; }
};
struct techi {
	const char*					id;
	const char*					name;
	tech_color_s				color;
	char						required[4]; // RGBY
};
struct actioni {
	const char*					id;
	const char*					name;
	int							count;
	play_s						type;
	bool(*proc)(playeri* p, bool run);
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
class answeri : stringbuilder {
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
	void						clear() { stringbuilder::clear(); elements.clear(); }
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
DECLENUM(tech);