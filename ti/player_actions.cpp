#include "main.h"

static void refresh_players() {
	for(auto& e : bsmeta<playeri>()) {
		e.set(StrategyAction, 1);
		e.set(TacticalAction, 1);
		e.set(Pass, 1);
	}
}

static planeti* choose_planet_construct(const playeri* p, variant_s type, const char* format) {
	planeta source;
	for(auto& e : bsmeta<planeti>()) {
		if(!e)
			continue;
		if(e.getplayer() != p)
			continue;
		if(type == PDS) {
			if(e.getcount(type, p) >= 2)
				continue;
		} else {
			if(e.getcount(type, p) >= 1)
				continue;
		}
		source.add(&e);
	}
	return p->choose(source, format, getstr(type));
}

static variant_s choose_dock_or_PDS(const playeri* p) {
	string sb; answeri ai;
	ai.add(SpaceDock, getstr(SpaceDock));
	ai.add(PDS, getstr(PDS));
	return (variant_s)p->choose(sb, ai, false, "Что вы хотите построить именно вы хотите построить в первую очередь?");
}

static void build_primary(playeri* p) {
	auto ut = choose_dock_or_PDS(p);
	auto pp = choose_planet_construct(p, ut, "Выбирайте планету, на которой будете строить [%1].");
	if(pp)
		p->create(ut, pp);
	pp = choose_planet_construct(p, PDS, "На втором шаге мы усилим свою безопасность. Где мы будем строить [%1]?");
	if(pp)
		p->create(PDS, pp);
}

static void trade_activity(playeri* p) {
	adat<playeri*, 8> checked;
	while(true) {
		string sb; answeri ai;
		sb.add("Мы можем заключить несколько торговых соглашений. Нам это ничего не даст, но оппонент оценит этот акт доброй воли.");
		if(checked) {
			auto first = false;
			for(auto pp : checked) {
				if(first)
					sb.adds("Вот список тех, с кем мы заключим соглашение:");
				else
					sb.adds(".");
				sb.adds(pp->getname());
			}
		} else
			sb.adds("На данный момент список кандидатов пустой.");
		for(auto& e : bsmeta<playeri>()) {
			if(!e || p == &e)
				continue;
			if(e.get(Commodities) == e.getcommodities())
				continue;
			if(checked.is(&e))
				ai.add((int)&e, "Исключить %1", e.getname());
			else
				ai.add((int)&e, e.getname());
		}
		if(!ai)
			break;
		ai.add(0, "Все верно");
		auto pp = (playeri*)p->choose(sb, ai, false, "Кого хотите включить или исключить в список?");
		if(!pp)
			break;
		if(checked.is(pp))
			checked.remove(checked.indexof(pp));
		else
			checked.add(pp);
	}
	for(auto pp : checked)
		pp->replenish_commodities();
}

static void	return_command_from_board(playeri* p) {
	string sb; answeri ai;
	solara source;
	p->select(source, Friendly | Activated);
	auto ps = (solari*)p->choose(source, "Выбирайте звездную систему, с которой вы заберете командный жетон и отряды в которой смогут действовать в этом ходу снова.");
	if(!ps)
		return;
	ps->deactivate(p);
	p->add_command_tokens(1);
}

static void redistribute_tokens(playeri* p) {
	while(true) {
		string sb; answeri ai;
		sb.add("Сложившая ситуация позволяет нам перерупировать свои силы.");
		for(auto i = Strategic; i <= Tactical; i = (action_s)(i + 1))
			ai.add(i, "Увеличить жетоны %1", getstr(i));
		ai.add(0, "Все корректно и так");
		auto n1 = (action_s)p->choose(sb, ai, false, "Вы можете перераспределить свои командный жетоны. Что вы хотите сделать?");
		sb.adds("Вы увеличите жетоны %1 до [%2i].", getstr(n1), p->get(n1) + 1);
		ai.clear();
		for(auto i = Strategic; i <= Tactical; i = (action_s)(i + 1)) {
			if(i == n1)
				continue;
			if(p->get(i) > 0)
				ai.add(i, "Уменьшить жетоны %1", getstr(i));
		}
		if(!ai)
			break;
		auto n2 = (action_s)p->choose(sb, ai, true, "За счет чего?");
		if(n2) {
			p->add(n1, 1);
			p->add(n2, -1);
		}
	}
}

static void tactical_action(playeri* p) {
	solara source; p->select(source, 0);
	auto solar = p->choose(source, "Выбирайте систему, которую вы хотите активировать");
	if(!solar)
		return;
	solar->activate(p);
	p->moveships(solar);
}

static void strategy_secondanary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		p->refresh_planets(1);
		break;
	case Politics:
		p->add_action_cards(1);
		break;
	case Trade:
		break;
	case Warfare:
		break;
	case Technology:
		p->buy_technology(4);
		break;
	case Imperial:
		p->build_units(1);
		break;
	}
}

static void strategy_primary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->add_command_tokens(3);
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		p->choose_diplomacy();
		break;
	case Politics:
		p->choose_speaker(1);
		p->add_action_cards(2);
		p->predict_next_political_card(2);
		break;
	case Construction:
		build_primary(p);
		break;
	case Trade:
		p->add_trade_goods(3);
		p->replenish_commodities();
		trade_activity(p);
		break;
	case Warfare:
		return_command_from_board(p);
		redistribute_tokens(p);
		break;
	case Technology:
		p->add_technology(1);
		p->buy_technology(6);
		break;
	case Imperial:
		p->add_objective(1);
		p->add_victory_points(2);
		break;
	}
}

static action_s choose_action(playeri* p, play_s play) {
	string sb; answeri ai;
	for(auto a = Armistice; a <= LastAction; a = (action_s)(a + 1)) {
		if(!p->is(a) || !p->isallow(play, a))
			continue;
		ai.add(a, getstr(a), getstr(p->strategy));
	}
	return (action_s)p->choose(sb, ai, false, "Что вы предпочитаете делать в свой ход?");
}

static void play_action(playeri* p, action_s id) {
	switch(id) {
	case StrategyAction:
		strategy_primary_action(p, p->strategy);
		break;
	case TacticalAction:
		tactical_action(p);
		break;
	}
}

static void select(playera& source, const playeri* start) {
	auto index = start->getid();
	for(auto& e : bsmeta<playeri>()) {
		source.add(&bsmeta<playeri>::elements[index++]);
		if(index >= (int)bsmeta<playeri>::count)
			index = 0;
	}
}

static void strategic_phase() {
	playera source;
	select(source, playeri::getspeaker());
	for(auto p : source)
		p->strategy = NoStrategy;
	adat<strategy_s, Imperial + 1> politics;
	for(auto i = Leadership; i <= Imperial; i = (strategy_s)(i + 1))
		politics.add(i);
	for(auto p : source) {
		string sb; sb.clear();
		answeri ai; ai.clear();
		p->activate();
		for(auto e : politics)
			ai.add(e, getstr(e));
		ai.sort();
		p->strategy = (strategy_s)p->choose(sb, ai, false,
			"Эта стратегическая фаза. "
			"Вам нужно выбрать одну стратегию из списка ниже, которую будете использовать на этот ход. "
			"Ваши оппоненты также выбирают одну стратегию из этого же списка.");
		sb.adds("Наш выбор [%-1] стратегия.", getstr(p->strategy));
		sb.adds(bsmeta<strategyi>::elements[p->strategy].text);
		p->message(sb);
		politics.remove(politics.indexof(p->strategy));
	}
}

static void action_phase() {
	const int last_initiative = 8;
	refresh_players();
	auto someone_move = true;
	while(someone_move) {
		someone_move = false;
		for(auto i = 0; i <= last_initiative; i++) {
			for(auto& e : bsmeta<playeri>()) {
				if(e.get(Pass) == 0)
					continue;
				if(e.getinitiative() != i)
					continue;
				e.activate();
				auto a = choose_action(&e, AsAction);
				play_action(&e, a);
				e.add(a, -1);
				someone_move = true;
			}
		}
	}
}

void playeri::make_move(bool strategic, bool action) {
	if(strategic)
		strategic_phase();
	if(action)
		action_phase();
}