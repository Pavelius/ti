#include "main.h"

static bool has_tactic(const playeri& player) {
	return player.get(Command) > 0;
}

static bool allow_pass(const playeri& player) {
	if(player.get(StrategyAction) > 0)
		return false;
	return true;
}

actioni bsmeta<actioni>::elements[] = {{"NoAction", "��� ��������"},
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
deck<action_s>	action_deck;

bool playeri::isallow(play_s type, action_s id) const {
	if(type != bsmeta<actioni>::elements[id].proc.type)
		return false;
	if(bsmeta<actioni>::elements[id].proc.test && !bsmeta<actioni>::elements[id].proc.test(*this))
		return false;
	return true;
}

void playeri::create_action_deck() {
	action_deck.clear();
	for(auto i = Armistice; i <= WarFooting; i=(action_s)(i+1)) {
		for(auto j = 0; j < bsmeta<actioni>::elements[i].count; j++)
			action_deck.add(i);
	}
	action_deck.shuffle();
}