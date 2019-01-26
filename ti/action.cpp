#include "main.h"

static bool has_tactic(const player_info* player) {
	return player->get(Command) > 0;
}

static bool allow_pass(const player_info* player) {
	if(player->get(StrategyAction) > 0)
		return false;
	return true;
}

struct action_info {
	typedef bool (*test_proc)(const player_info* player);
	struct proc_info {
		play_s		type;
		test_proc	test;
		constexpr proc_info() : type(NoPlay), test(0) {}
		constexpr proc_info(play_s type) : type(type), test(0) {}
		constexpr proc_info(test_proc proc) : type(AsAction), test(proc) {}
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
{"StrategyAction", "%1 ���������", 0, AsAction, ""},
{"TacticalAction", "����������� ��������", 0, has_tactic, ""},
{"TransferAction", "�����������", 0, has_tactic, ""},
{"Pass", "������� ����", 0, allow_pass, ""},
//
{"CombatEquipment", "������� ����������", 0, 0, ""},
{"TradeAction", "��������� ����������", 0, 0, ""},
{"RerollCombatDices", "����������� ��� �������", 0, 0, ""},
{"MentakAmbush", "����� ����������", 0, 0, ""},
{"MentakPillage", "���������", 0, 0, ""},
{"FleetRetreat", "��������� ����� ������", 0, 0, ""},
{"OrbitalDrop", "����������� �������", 0, 0, ""},
{"ExecutePrimaryAbility", "��������� ��������� �����������", 0, 0, ""},
{"ChangePoliticCard", "�������� �����", 0, 0, ""},
{"LookActionCard", "���������� �������� ��������", 0, 0, ""},
//
{"Strategy", "���������", 0, NoPlay, ""},
{"Fleet", "�����", 0, NoPlay, ""},
{"Command", "�������", 0, NoPlay, ""},
{"Goods", "������", 0, NoPlay, ""},
};
assert_enum(action, LastAction);
getstr_enum(action);

deck<action_s>	action_deck;

bool player_info::isallow(play_s type, action_s id) const {
	if(type != action_data[id].proc.type)
		return false;
	if(action_data[id].proc.test && !action_data[id].proc.test(this))
		return false;
	return true;
}

void player_info::create_action_deck() {
	action_deck.clear();
	for(auto i = Armistice; i <= WarFooting; i=(action_s)(i+1)) {
		for(auto j = 0; j < action_data[i].count; j++)
			action_deck.add(i);
	}
	action_deck.shuffle();
}