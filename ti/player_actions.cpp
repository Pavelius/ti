#include "main.h"

static const char* number_which[] = {"нулевую", "первую", "вторую", "третью", "четвертую", "пятую", "шестую"};

static const int last_initiative = 8;

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
	string sb;
	sb.adds(format, getstr(type));
	return p->choose(source, sb);
}

static variant_s choose_dock_or_PDS(const playeri* p) {
	answeri ai;
	ai.add(SpaceDock, getstr(SpaceDock));
	ai.add(PDS, getstr(PDS));
	return (variant_s)p->choose(ai, false, "Что вы хотите построить именно вы хотите построить в первую очередь?");
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
		sb.adds("Кого хотите включить или исключить в список?");
		auto pp = (playeri*)p->choose(ai, false, sb);
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
		answeri ai;
		for(auto i = Strategic; i <= Tactical; i = (action_s)(i + 1))
			ai.add(i, "Увеличить жетоны %1", getstr(i));
		ai.add(0, "Все корректно и так");
		string sb;
		sb.add("Сложившая ситуация позволяет нам перерупировать свои силы.");
		auto ps = sb.get();
		sb.add("Вы можете перераспределить свои командный жетоны. Что вы хотите сделать?");
		auto n1 = (action_s)p->choose(ai, false, sb);
		sb.set(ps);
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
		auto n2 = (action_s)p->choose(ai, true, "За счет чего?");
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

static void score_public_objective(int value) {
}

static bool allow_refresh_planets(const playeri* p) {
	planeta source; p->select(source, Friendly | Activated);
	return source.getcount()!=0;
}

static bool allow_gain_action_cards(const playeri* p) {
	return true;
}

static bool allow_gain_commodities(const playeri* p) {
	return p->getcommodities()!=p->get(Commodities);
}

static void refresh_planets(playeri* p, int value) {
	for(auto i = 1; i <= value; i++) {
		planeta source; p->select(source, Friendly | Activated);
		if(!source)
			break;
		string sb;
		sb.add("Выбирайте [%1] планету, ресурсы которой будут обновлены", maptbl(number_which,i));
		auto pp = p->choose(source, sb);
		pp->remove(Exhaused);
	}
}

static void strategy_secondanary_action(playeri* p, strategy_s id) {
	switch(id) {
	case Leadership:
		p->buy_command_tokens(3);
		break;
	case Diplomacy:
		if(allow_refresh_planets(p)) {
			auto n = p->pay(1, 1, "обновление двух планет", "обновления двух планет", Strategic);
			if(n)
				refresh_planets(p, 2);
		}
		break;
	case Politics:
		if(allow_gain_action_cards(p)) {
			auto n = p->pay(1, 1, "2 карты действия", "2 карты действия", Strategic);
			if(n)
				p->add_action_cards(2);
		}
		break;
	case Trade:
		if(allow_gain_commodities(p)) {
			auto n = p->pay(1, 1, "обновление продукции", 0, Strategic);
			if(n)
				p->replenish_commodities();
		}
		break;
	case Warfare:
		break;
	case Technology:
		p->buy_technology(4);
		break;
	case Imperial:
		//p->build_units(1);
		break;
	}
}

static void strategy_primary_action(playeri* p, strategy_s id, bool allow_secondanary) {
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
		score_public_objective(1);
		if(solari::getmekatol()->getplayer() == p)
			p->add_victory_points(1);
		else
			p->add_secret_objective(1);
		break;
	}
	if(allow_secondanary) {
		auto pp = p + 1;
		while(p != pp) {
			if(pp >= bsmeta<playeri>::elements + bsmeta<playeri>::count)
				pp = bsmeta<playeri>::elements;
			strategy_secondanary_action(pp, id);
			pp++;
		}
	}
}

static action_s choose_action(playeri* p, play_s play) {
	answeri ai;
	for(auto a = Armistice; a <= LastAction; a = (action_s)(a + 1)) {
		if(!p->is(a) || !p->isallow(play, a))
			continue;
		ai.add(a, getstr(a), getstr(p->strategy));
	}
	return (action_s)p->choose(ai, false, "Что вы предпочитаете делать в свой ход?");
}

static void play_action(playeri* p, action_s id) {
	switch(id) {
	case StrategyAction:
		strategy_primary_action(p, p->strategy, true);
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
		// sb.clear();
		answeri ai; ai.clear();
		p->activate();
		for(auto e : politics)
			ai.add(e, getstr(e));
		ai.sort();
		p->strategy = (strategy_s)p->choose(ai, false,
			"Эта стратегическая фаза. "
			"Вам нужно выбрать одну стратегию из списка ниже, которую будете использовать на этот ход. "
			"Ваши оппоненты также выбирают одну стратегию из этого же списка.");
		string sb;
		sb.adds("Наш выбор [%-1] стратегия.", getstr(p->strategy));
		sb.adds(bsmeta<strategyi>::elements[p->strategy].text);
		p->message(sb);
		politics.remove(politics.indexof(p->strategy));
	}
}

static void action_phase() {
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