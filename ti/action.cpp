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
	if(run) {
		string sb;
		sb.add("На этом наш ход закончен. Будем ждать следующего хода.");
		p->set(Pass, 0);
		p->apply(sb);
	}
	return true;
}

static bool action_strategy(playeri* p, bool run) {
	if(p->get(StrategyAction) <= 0)
		return false;
	if(run)
		strategy_primary_action(p, p->strategy, true);
	return true;
}

INSTDATA(actioni) = {{"NoAction", "Нет действия"},
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
{"StrategyAction", "%1 стратегия", 0, AsAction, action_strategy},
{"TacticalAction", "Тактическое действие", 0, AsAction, action_tactic},
{"Pass", "Пропуск хода", 0, AsAction, action_pass},
//
{"Strategy", "стратегии"},
{"Fleet", "флота"},
{"Command", "тактики"},
//
{"VictoryPoints", "победных очков"},
//
{"Commodities", "Продукция"},
{"Goods", "Товары"},
};
assert_enum(action, LastAction)
deck<action_s>	action_deck;

bool playeri::isallow(play_s type, action_s id) const {
	auto& e = bsdata<actioni>::elements[id];
	if(type != e.type)
		return false;
	if(e.proc && !e.proc(const_cast<playeri*>(this), false))
		return false;
	return true;
}

void playeri::create_action_deck() {
	action_deck.clear();
	for(auto i = NoAction; i <= LastAction; i=(action_s)(i+1)) {
		for(auto j = 0; j < bsdata<actioni>::elements[i].count; j++)
			action_deck.add(i);
	}
	action_deck.shuffle();
}