#include "main.h"

void strategy_primary_action(playeri* p, strategy_s id, bool allow_secondanary);
void tactical_action(playeri* p);

static bool action_tactic(playeri* p, bool run) {
	if(p->get(Tactical) <= 0)
		return false;
	if(run)
		tactical_action(p);
	return true;
}

static bool action_pass(playeri* p, bool run) {
	if(p->ispassed())
		return false;
	if(p->get(StrategyAction) > 0)
		return false;
	if(run)
		p->set(Pass, 0);
	return true;
}

static bool action_strategy(playeri* p, bool run) {
	if(p->get(StrategyAction) <= 0)
		return false;
	if(run)
		strategy_primary_action(p, p->strategy, true);
	return true;
}

actioni bsmeta<actioni>::elements[] = {{"NoAction", "Нет действия"},
{"AncientBurialSites", "Руины древних цивилизаций", 1, BeforeAgendaPhase},
{"AssassinateRepresentative", "Убийство посла", 1, AfterAgendaRevealed},
{"Bunker", "Бункер", 1, BeforeBombardment},
{"CrippleDefenses", "Подрыв обороноспособности", 1, AsAction},
{"DirectHit", "Прямое попадание", 4, AfterSustainDamage},
{"Sabotage", "Саботаж", 4},
{"Spy", "Шпионаж", 1, AsAction},
{"Uprising", "Восстание", 1, AsAction},
{"WarfareRider", "", 1, AsAction},
//
{"CombatEquipment", "Тяжелое вооружение"},
{"TradeAction", "Торговать карточками"},
{"RerollCombatDices", "Перебросить все провалы"},
{"MentakAmbush", "Налет крейсерами"},
{"MentakPillage", "Пиратство"},
{"FleetRetreat", "Отступить перед битвой"},
{"OrbitalDrop", "Орбитальная высадка"},
{"ExecutePrimaryAbility", "Выполнить первичную способность"},
{"ChangePoliticCard", "Поменять закон"},
{"LookActionCard", "Посмотреть карточки действий"},
//
{"StrategyAction", "%1 стратегия", 0, AsAction, action_strategy},
{"TacticalAction", "Тактическое действие", 0, AsAction, action_tactic},
{"Pass", "Пропуск хода", 0, AsAction, action_pass},
//
{"Strategy", "стратегии", 0, AsAction, action_strategy},
{"Fleet", "флота"},
{"Command", "тактики"},
//
{"VictoryPoints", "победных очков"},
//
{"Commodities", "Продукция"},
{"Goods", "Товары"},
};
assert_enum(action, LastAction);
deck<action_s>	action_deck;

bool playeri::isallow(play_s type, action_s id) const {
	auto& e = bsmeta<actioni>::elements[id];
	if(type != e.type)
		return false;
	if(e.proc && !e.proc(const_cast<playeri*>(this), false))
		return false;
	return true;
}

void playeri::create_action_deck() {
	action_deck.clear();
	for(auto i = NoAction; i <= LastAction; i=(action_s)(i+1)) {
		for(auto j = 0; j < bsmeta<actioni>::elements[i].count; j++)
			action_deck.add(i);
	}
	action_deck.shuffle();
}