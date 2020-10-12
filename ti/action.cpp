#include "main.h"

void strategy_primary_action(playeri* p, strategy_s id, bool allow_secondanary);

INSTDATA(actioni) = {{"AncientBurialSites", "Руины древних цивилизаций", 1, BeforeAgendaPhase},
{"AssassinateRepresentative", "Убийство посла", 1, AfterAgendaRevealed},
{"Bunker", "Бункер", 1, BeforeBombardment},
{"CrippleDefenses", "Подрыв обороноспособности", 1, AsAction},
{"DirectHit", "Прямое попадание", 4, AfterSustainDamage},
{"Sabotage", "Саботаж", 4},
{"Spy", "Шпионаж", 1, AsAction},
{"Uprising", "Восстание", 1, AsAction},
{"WarfareRider", "", 1, AsAction},
//
{"OrbitalDrop", "Орбитальная высадка", 0, AsAction, {PayStrategy}, {{"Выбирайте планету, на которую будет осуществлена высадка войск", Friendly}}, 2, "Мы высадили с орбиты [%1i] наземных отряда на свою планету."},
//
{"StrategyAction", "%1 стратегия", 0, AsAction},
{"TacticalAction", "Тактическое действие", 0, AsAction},
{"Pass", "Пропуск хода", 0, AsAction, {}, {}, 0, "На этом наш ход закончен. Будем ждать следующего хода."},
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

void playeri::create_action_deck() {
	action_deck.clear();
	for(auto i = FirstActionCard; i <= LastAction; i=(action_s)(i+1)) {
		for(auto j = 0; j < bsdata<actioni>::elements[i].count; j++)
			action_deck.add(i);
	}
	action_deck.shuffle();
}

bool playeri::play(action_s id, bool run) {
	auto& e = bsdata<actioni>::elements[id];
	if(e.is(PayStrategy) && get(StrategyAction) <= 0)
		return false;
	auto effect_count = e.effect_count;
	planeta planets; planeti* planet = 0;
	solara solars; solari* solar = 0;
	if(e.stages.planet) {
		select(planets, e.stages.planet.flags);
		if(!planets)
			return false;
		if(run)
			planet = choose(planets, e.stages.planet.text);
	}
	switch(id) {
	case OrbitalDrop:
		if(run) {
			create(GroundForces, planet);
			create(GroundForces, planet);
		}
		break;
	case StrategyAction:
		if(get(StrategyAction) <= 0)
			return false;
		if(run)
			strategy_primary_action(this, strategy, true);
		break;
	case TacticalAction:
		if(get(Tactical) <= 0)
			return false;
		if(run)
			tactical();
		break;
	case Pass:
		if(ispassed())
			return false;
		if(get(StrategyAction) > 0)
			return false;
		if(run)
			set(Pass, 0);
		break;
	}
	if(run) {
		if(e.effect_text) {
			string sb;
			sb.add(e.effect_text, e.effect_count);
			apply(sb);
		}
	}
	return true;
}