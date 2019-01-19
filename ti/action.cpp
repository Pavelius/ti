#include "main.h"

enum play_s : unsigned char {
	NoPlay,
	AsAction, StrategicPhase,
	AfterByingTechnology,
	BeforeDrawPoliticCard, BeforeInvasion, BeforeSpaceCombat, BeforeCombatRound,
	BeforeStrategy, AfterHit,
};
struct action_info {
	struct proc_info {
		play_s		type;
		constexpr proc_info(play_s type) : type(type) {}
		constexpr proc_info() : type(AsAction) {}
	};
	const char*		id;
	const char*		name;
	int				count;
	proc_info		proc;
	const char*		description;
};
static action_info action_data[] = {{"NoAction", "��� ��������"},
{"Armistice", "���������", 1, AsAction, "�������� ��������� � �������, ����������� ��� ����� ���������. ������ ����� �� ����� ��������� ��������� �� ��������� ������� � ���� �����."},
{"ChemicalWarfare", "", 1, BeforeInvasion, "������� ������ ������, ���� �� ������ �������� � �������, ���������� �������, ����������� ��� ��������� ���������. ���������� �������� ������ (��������� � ������� �������). ��������� ���� ��� ������� �� ������� PDS."},
{"CivilDefense", "", 1, AsAction, "����������� ��� ����� ����������� ������� �� ���� �������, ������� �� �������� ������ ������� ����."},
{"CommandSummit", "", 1, StrategicPhase, "�������� ��� ��������� �������."},
{"CorporateSponsorship", "", 1, AfterByingTechnology, "�� ������ ���������� ������� ���������� �� ������� � 4 �������."},
{"CouncilDissolved", "", 1, BeforeStrategy, "�����, ��������� ������������ ��������� �� ����� �� ������ ������������ �����."},
{"CulturalCrisis", "", 1, StrategicPhase, "�������� ������ (����� ����). ������ ����� ������ � ������ ����� ��� ����������� ����� ����."},
{"DeterminePolicy", "", 1, BeforeDrawPoliticCard, "�������� ���� ����� �� ������ ������������ ����, �� ������ ���������� ���� ��� �� ����������� �������.����� ������ ����� ���������� �� ������ �����, � �� ����� ����� ������ �� ������������ ������. ���� �� ����� ����� �� ������������ ������, ����������� ��."},
{"DiplomaticImmunity", "", 1, AsAction, "�������� �������, � ������� �� ������ ���� �� ���� �������. ������ �� ������ ������������ ������ ������� � ���� ���."},
{"DirectHit", "", 4, AfterHit, "���������� ������������ ������� �����."},
{"Disclosure", "", 1, AsAction, "���������� ����� �������� ���������. �������� � �������� ���� �� ����."},
{"Discredit", "�������������", 1, AsAction, "������ ���������� ������ �� �����������."},
{"DugIn", "��������", 1, AsAction, "�������� �������. ���� �������� ���� �� ������ ������� �� ���������� ������������� � ������ �����."},
{"EmergencyRepairs", "��������� ������", 2, AsAction, "�������� �������. ���������� �������������� ��� ���� ��������� � ������� ������ � ��������� �������."},
{"ExperimentalBattlestation", "����������������� ������ �������", 1, AsAction, "�������� ��� ���. ��� ����� ���������� ��������� ��� ���� ������, �������� ��� PDS."},
{"FantasticRhetoric", "�������������� ��������", 1, AsAction, "�� ��������� �������������� 10 �������."},
{"FighterPrototype", "�������� �����������", 1, AsAction, "�������� �������. ��� ����������� � ������ ������� �������� ����������� + 2 �� ���� ������� ������� ����� �� ���� �����."},
{"FlankSpeed", "��������� �����", 4, AsAction, "�������� �������, ������� �� ������ ��� ������������ � ������� ���������� �������.��������� �������� ���� �������� �������������� � ������ ������� �� 1."},
{"FocusedResearch", "���������������� ������������", 1, AsAction, "������� 6 �������� ��� ������������� ������ ���������������� ������������ �� ������ ����������."},
{"GhostShip", "�������-�������", 1, AsAction, "����������� ��������� ������� � ���������� �������, ���������� �������, � ��� ��� ����� ��������."},
{"GoodYear", "��������� ���", 1, AsAction, "�������� ���� ������ ������ �� ������ �������, ������� �� �������������, � ������� ��������� �� � ����� �������� �������."},
{"GrandArmada", "������� ������", 1, AsAction, "�� ��� ����, ����������� � ����� �������� ������� � ������ ��� �� ���������������� ������� �� ����������� ��������. � ����� ���� ������� �� ������ ������ � ����� ��� �������, ����������� ������� �����������."},
{"InTheSilenceOfSpace", "� ������ �������", 3, AsAction, "�������� ���� �� ����� ������. ��� ������� ����� �� ��������� 2 � ���� ����� ������, �� �� ���������������, ����� ������� ������� ������ ���������."},
{"InfluenceInTheMerchantsGuild", "", 1, AsAction, ""},
{"Insubordination", "", 1, AsAction, ""},
{"IntoTheBreach", "", 1, AsAction, ""},
{"LocalUnrest", "", 4, AsAction, ""},
{"LuckyShot", "", 1, AsAction, ""},
{"MassiveTransport", "", 1, AsAction, ""},
{"MasterOfTrade", "", 1, AsAction, ""},
{"Minelayers", "", 1, AsAction, ""},
{"MoraleBoost", "", 5, AsAction, ""},
{"Multiculturalism", "", 1, AsAction, ""},
{"OpeningTheBlackBox", "", 1, AsAction, ""},
{"Patrol", "", 1, AsAction, ""},
{"Plague", "", 1, AsAction, ""},
{"PolicyParalysis", "", 1, AsAction, ""},
{"PoliticalStability", "", 1, AsAction, ""},
{"Privateers", "", 1, AsAction, ""},
{"ProductivitySpike", "", 1, AsAction, ""},
{"PublicDisgrace", "", 1, AsAction, ""},
{"RallyOfThePeople", "", 1, AsAction, ""},
{"RareMineral", "", 3, AsAction, ""},
{"Recheck", "", 4, AsAction, ""},
{"Reparations", "", 1, AsAction, ""},
{"RiseOfAMessiah", "", 1, AsAction, ""},
{"RuinousTariffs", "", 1, AsAction, ""},
{"Sabotage", "", 5, AsAction, ""},
{"ScientistAssassination", "", 1, AsAction, ""},
{"SecretIndustrialAgent", "", 1, AsAction, ""},
{"ShieldsHolding", "", 1, AsAction, ""},
{"SignalJamming", "", 4, AsAction, ""},
{"SkilledRetreat", "", 4, AsAction, ""},
{"SpacedockAccident", "", 1, AsAction, ""},
{"StarOfDeath", "", 1, AsAction, ""},
{"StellarCriminals", "", 1, AsAction, ""},
{"StrategicBombardment", "", 1, AsAction, ""},
{"StrategicFlexibility", "", 1, AsAction, ""},
{"StrategicShift", "", 1, AsAction, ""},
{"SuccessfulSpy", "", 1, AsAction, ""},
{"Synchronicity", "", 1, AsAction, ""},
{"TechBubble", "", 1, AsAction, ""},
{"TouchOfGenius", "", 1, AsAction, ""},
{"TradeStop", "", 1, AsAction, ""},
{"Transport", "", 1, AsAction, ""},
{"Thugs", "", 1, AsAction, ""},
{"UnexpectedAction", "", 1, AsAction, ""},
{"Uprising", "", 1, AsAction, ""},
{"Usurper", "", 1, AsAction, ""},
{"Veto", "", 1, AsAction, ""},
{"VoluntaryAnnexation", "", 1, AsAction, ""},
{"WarFooting", "", 1, AsAction, ""},
//
{"StrategyAction", "%1 ���������", 1, AsAction, ""},
{"TacticalAction", "����������� ��������", 1, AsAction, ""},
{"TransferAction", "�����������", 1, AsAction, ""},
{"Pass", "������� ����", 1, AsAction, ""},
//
{"Strategy", "���������", 0, NoPlay, ""},
{"Fleet", "�����", 0, NoPlay, ""},
{"Command", "�������", 0, NoPlay, ""},
{"Goods", "������", 0, NoPlay, ""},
};
assert_enum(action, LastAction);
getstr_enum(action);