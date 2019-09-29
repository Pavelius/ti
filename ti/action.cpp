#include "main.h"

static bool has_tactic(const playeri& player) {
	return player.get(Command) > 0;
}

static bool allow_pass(const playeri& player) {
	if(player.get(StrategyAction) > 0)
		return false;
	return true;
}

actioni bsmeta<actioni>::elements[] = {{"NoAction", "Нет действия"},
{"Armistice", "Перемирие", 1, AsAction, "Выберите оппонента и планету, находящуюся под вашим контролем. Данный игрок не может совершать вторжение на выбранную планету в этот раунд."},
{"ChemicalWarfare", "", 1, BeforeInvasion, "Играйте данной картой, если вы имеете дредноут в системе, содержащей планету, находящуюся под контролем оппонента. Уничтожьте половину пехоты (округлить в большую сторону). Действует даже при наличии на планете PDS."},
{"CivilDefense", "", 1, AsAction, "Расположите два юнита планетарной обороны на вашу планету, которая не содержит юнитов данного типа."},
{"CommandSummit", "", 1, StrategicPhase, "Получите два командных маркера."},
{"CorporateSponsorship", "", 1, AfterByingTechnology, "Вы можете приобрести зеленую технологию со скидкой в 4 ресурса."},
{"CouncilDissolved", "", 1, BeforeStrategy, "Игрок, выбравший Политическую стратегию не берет из колоды политическую карту."},
{"CulturalCrisis", "", 1, StrategicPhase, "Выберите игрока (можно себя). Данный игрок теряет в данный раунд все особенности своей расы."},
{"DeterminePolicy", "", 1, BeforeDrawPoliticCard, "Выберите один закон из колоды политических карт, из колоды сброшенных карт или из действующих законов.Сенат должен вновь голосовать за данный закон, а не брать карту закона из политической колоды. Если вы взяли карту из политической колоды, перетасуйте ее."},
{"DiplomaticImmunity", "", 1, AsAction, "Выберите систему, в которой вы имеете хотя бы один корабль. Только вы можете активировать данную систему в этот ход."},
{"DirectHit", "", 4, AfterHit, "Уничтожьте поврежденный корабль врага."},
{"Disclosure", "", 1, AsAction, "Посмотрите карты действия оппонента. Выберите и сбросьте одну из карт."},
{"Discredit", "Дискредитация", 1, AsAction, "Голоса выбранного игрока не учитываются."},
{"DugIn", "Зарылись", 1, AsAction, "Выберите планету. Ваши наземные силы на данной планете не подвержены бомбардировки в данный раунд."},
{"EmergencyRepairs", "Авральный ремонт", 2, AsAction, "Выберите систему. Немедленно отремонтируйте все ваши дредноуты и военные звезды в выбранной системе."},
{"ExperimentalBattlestation", "Экспериментальная боевая станция", 1, AsAction, "Выберите ваш док. Док может немедленно атаковать три раза подряд, стреляет как PDS."},
{"FantasticRhetoric", "Фантастическая риторика", 1, AsAction, "Вы получаете дополнительные 10 голосов."},
{"FighterPrototype", "Прототип истребителя", 1, AsAction, "Выберите систему. Все истребители в данной системе получают модификатор + 2 во всех бросках кубиков атаки на один раунд."},
{"FlankSpeed", "Фланговый обход", 4, AsAction, "Выберите систему, которую вы только что активировали с помощью командного маркера.Увеличьте скорость всех кораблей перемещающихся в данную систему на 1."},
{"FocusedResearch", "Целенаправленное исследование", 1, AsAction, "Потрать 6 ресурсов для игнорирования одного технологического пререквизита на дереве технологий."},
{"GhostShip", "Корабль-призрак", 1, AsAction, "Расположите бесплатно эсминец в недомашней системе, содержащей вормхол, и где нет чужих кораблей."},
{"GoodYear", "Урожайный год", 1, AsAction, "Получите один маркер товара за каждую планету, которую вы контролируете, и которая находится не в вашей домашней системе."},
{"GrandArmada", "Великая армада", 1, AsAction, "На ваш флот, находящийся в вашей домашней системе в данный ход не распространяется правило на ограничение кораблей. В конце фазы статуса вы должны убрать с доски все корабли, превышающие правило ограничения."},
{"InTheSilenceOfSpace", "В тишине космоса", 3, AsAction, "Выберите один из ваших флотов. Все корабли флота со скоростью 2 и выше могут пройти, но не останавливаться, через систему занятую флотом оппонента."},
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
{"StrategyAction", "%1 стратегия", 0, AsAction, ""},
{"TacticalAction", "Тактическое действие", 0, has_tactic, ""},
{"TransferAction", "Перемещение", 0, has_tactic, ""},
{"Pass", "Пропуск хода", 0, allow_pass, ""},
//
{"CombatEquipment", "Тяжелое вооружение", 0, 0, ""},
{"TradeAction", "Торговать карточками", 0, 0, ""},
{"RerollCombatDices", "Перебросить все провалы", 0, 0, ""},
{"MentakAmbush", "Налет крейсерами", 0, 0, ""},
{"MentakPillage", "Пиратство", 0, 0, ""},
{"FleetRetreat", "Отступить перед битвой", 0, 0, ""},
{"OrbitalDrop", "Орбитальная высадка", 0, 0, ""},
{"ExecutePrimaryAbility", "Выполнить первичную способность", 0, 0, ""},
{"ChangePoliticCard", "Поменять закон", 0, 0, ""},
{"LookActionCard", "Посмотреть карточки действий", 0, 0, ""},
//
{"Strategy", "стратегии", 0, NoPlay, ""},
{"Fleet", "флота", 0, NoPlay, ""},
{"Command", "тактики", 0, NoPlay, ""},
{"Goods", "товары", 0, NoPlay, ""},
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