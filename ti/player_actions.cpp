#include "main.h"

static const char* number_which[] = {"�������", "������", "������", "������", "���������", "�����", "������"};

static const int last_initiative = 8;

static void refresh_players() {
	for(auto& e : bsdata<playeri>()) {
		e.set(StrategyAction, 1);
		e.set(TacticalAction, 1);
		e.set(Pass, 1);
	}
}

static planeti* choose_planet_construct(const playeri* p, variant_s type, const char* format) {
	planeta source;
	for(auto& e : bsdata<planeti>()) {
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
	sb.adds(format, bsdata<varianti>::elements[type].name);
	return p->choose(source, sb);
}

static variant_s choose_dock_or_PDS(const playeri* p) {
	answeri ai;
	ai.add(SpaceDock, getstr(SpaceDock));
	ai.add(PDS, getstr(PDS));
	return (variant_s)p->choose(ai, false, "��� �� ������ ��������� ������ �� ������ ��������� � ������ �������?");
}

static void build_primary(playeri* p) {
	auto ut = choose_dock_or_PDS(p);
	auto pp = choose_planet_construct(p, ut, "��������� �������, �� ������� ������ ������� [%1].");
	if(pp)
		p->add(ut, pp);
	pp = choose_planet_construct(p, PDS, "�� ������ ���� �� ������ ���� ������������. ��� �� ����� ������� [%1]?");
	if(pp)
		p->add(PDS, pp);
}

static void trade_activity(playeri* p) {
	adat<playeri*, 8> checked;
	while(true) {
		string sb; answeri ai;
		sb.add("�� ����� ��������� ��������� �������� ����������. ��� ��� ������ �� ����, �� �������� ������ ���� ��� ������ ����.");
		if(checked) {
			auto first = false;
			for(auto pp : checked) {
				if(first)
					sb.adds("��� ������ ���, � ��� �� �������� ����������:");
				else
					sb.adds(".");
				sb.adds(pp->getname());
			}
		} else
			sb.adds("�� ������ ������ ������ ���������� ������.");
		for(auto& e : bsdata<playeri>()) {
			if(!e || p == &e)
				continue;
			if(e.get(Commodities) == e.getcommodities())
				continue;
			if(checked.is(&e))
				ai.add((int)&e, "��������� %1", e.getname());
			else
				ai.add((int)&e, e.getname());
		}
		if(!ai)
			break;
		ai.add(0, "��� �����");
		sb.adds("���� ������ �������� ��� ��������� � ������?");
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
	auto ps = (solari*)p->choose(source, "��������� �������� �������, � ������� �� �������� ��������� ����� � ������ � ������� ������ ����������� � ���� ���� �����.");
	if(!ps)
		return;
	ps->deactivate(p);
	p->add_command_tokens(1);
}

static void redistribute_tokens(playeri* p) {
	while(true) {
		answeri ai;
		for(auto i = Strategic; i <= Tactical; i = (action_s)(i + 1))
			ai.add(i, "��������� ������ %1", getstr(i));
		ai.add(0, "��� ��������� � ���");
		string sb;
		sb.add("��������� �������� ��������� ��� �������������� ���� ����.");
		auto ps = sb.get();
		sb.add("�� ������ ���������������� ���� ��������� ������. ��� �� ������ �������?");
		auto n1 = (action_s)p->choose(ai, false, sb);
		sb.set(ps);
		sb.adds("�� ��������� ������ %1 �� [%2i].", getstr(n1), p->get(n1) + 1);
		ai.clear();
		for(auto i = Strategic; i <= Tactical; i = (action_s)(i + 1)) {
			if(i == n1)
				continue;
			if(p->get(i) > 0)
				ai.add(i, "��������� ������ %1", getstr(i));
		}
		if(!ai)
			break;
		auto n2 = (action_s)p->choose(ai, true, "�� ���� ����?");
		if(n2) {
			p->add(n1, 1);
			p->add(n2, -1);
		}
	}
}

static solari* activate_system(playeri* p) {
	solara source; p->selecta(source);
	return p->choose(source, "��������� �������, ������� �� ������ ������������");
}

planeti* playeri::choose_planet(const char* format, unsigned flags) {
	planeta source; select(source, flags);
	return choose(source, format);
}

static void after_activation(playeri* p, solari* solar) {
	string sb;
	sb.add("��������� ���� �������� �� �������� � ������� [%1].", solar->getname());
	p->apply(sb);
}

void playeri::tactical() {
	auto solar = activate_system(this);
	if(!solar)
		return;
	solar->activate(this);
	add(Tactical, -1);
	after_activation(this, solar);
	moveships(solar);
	build_units(solar);
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
		sb.add("��������� [%1] �������, ������� ������� ����� ���������", maptbl(number_which,i));
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
			auto n = p->pay(1, 1, "���������� ���� ������", "���������� ���� ������", Strategic);
			if(n) {
				p->add(Strategic, -1);
				refresh_planets(p, 2);
			}
		}
		break;
	case Politics:
		if(allow_gain_action_cards(p)) {
			auto n = p->pay(1, 1, "2 ����� ��������", "2 ����� ��������", Strategic);
			if(n) {
				p->add(Strategic, -1);
				p->add_action_cards(2);
			}
		}
		break;
	case Construction:
		break;
	case Trade:
		if(allow_gain_commodities(p)) {
			auto n = p->pay(1, 1, "���������� ���������", 0, Strategic);
			if(n) {
				p->add(Strategic, -1);
				p->replenish_commodities();
			}
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

void strategy_primary_action(playeri* p, strategy_s id, bool allow_secondanary) {
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
	p->set(StrategyAction, 0);
	if(allow_secondanary) {
		auto pp = p + 1;
		while(p != pp) {
			strategy_secondanary_action(pp, id);
			pp++;
			if(pp >= (playeri*)bsdata<playeri>::source.end())
				pp = (playeri*)bsdata<playeri>::source.begin();
		}
	}
}

static action_s choose_action(playeri* p, play_s play) {
	answeri ai;
	for(auto a = FirstActionCard; a <= LastAction; a = (action_s)(a + 1)) {
		if(bsdata<actioni>::elements[a].type != play)
			continue;
		if(!p->is(a) || !p->play(a, false))
			continue;
		switch(a) {
		case StrategyAction:
			ai.add(a, getstr(a), getstr(p->strategy));
			break;
		default:
			ai.add(a, getstr(a));
			break;
		}
	}
	return (action_s)p->choose(ai, false, "��� �� ������������� ������ � ���� ���?");
}

static void select(playera& source, const playeri* start) {
	auto index = start->getid();
	for(auto& e : bsdata<playeri>()) {
		source.add(&bsdata<playeri>::elements[index++]);
		if(index >= (int)bsdata<playeri>::source.getcount())
			index = 0;
	}
}

static void strategy_tips(stringbuilder& sb, int param) {
	auto& e = bsdata<strategyi>::elements[param];
	sb.add(e.text);
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
			"��� �������������� ����. "
			"��� ����� ������� ���� ��������� �� ������ ����, ������� ������ ������������ �� ���� ���. "
			"���� ��������� ����� �������� ���� ��������� �� ����� �� ������.",
			strategy_tips);
		string sb;
		sb.adds("��� ����� [%-1] ���������.", getstr(p->strategy));
		sb.adds(bsdata<strategyi>::elements[p->strategy].text);
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
			for(auto& e : bsdata<playeri>()) {
				if(e.get(Pass) == 0)
					continue;
				if(e.getinitiative() != i)
					continue;
				e.activate();
				auto a = choose_action(&e, AsAction);
				e.play(a, true);
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