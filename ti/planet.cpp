#include "main.h"

template<> solari bsdata<solari>::elements[48];
template<> array bsdata<solari>::source(bsdata<solari>::elements, sizeof(bsdata<solari>::elements[0]),
	sizeof(bsdata<solari>::elements) / sizeof(bsdata<solari>::elements[0]),
	sizeof(bsdata<solari>::elements) / sizeof(bsdata<solari>::elements[0]));
static unsigned char solar_indecies[map_scan_line * map_scan_line];
static unsigned char movement_rate[map_scan_line * map_scan_line];

INSTDATA(planeti) = {{"����� ���", "xxcha", 2, 3, 0},
{"��� �����", "barony", 4, 0, 0},
{"������", "naalu", 0, 2, 0},
{"[0.0.0]", "mindnet", 5, 0, 0},
{"���������", "yssaril", 2, 3, 0},
{"����", "sol", 4, 2, 0},
{"��� ������", "mentak", 4, 1, 0},
{"�����", "hacan", 2, 0, 0},
{"���", "jelnar", 2, 1, 0},
{"����'���", "norr", 1, 0, 0},
//
{"����� ���", "xxcha", 1, 1, 5},
{"���� �����", "barony", 2, 1, 5},
{"����", "naalu", 3, 1, 5},
{"������", "yssaril", 1, 2, 5},
{"�������", "hacan", 1, 1, 5},
{"���", "jelnar", 1, 2, 5},
{"��������", "norr", 3, 1, 5},
//
{"�������", "hacan", 0, 1, 2},
//
{"������� ����", 0, 1, 6, 16, NoTech},
{"�����", 1, 3, 1, 12, Green},
{"��������", 2, 1, 3, 13, Blue},
{"������'�", 3, 1, 2, 2, Green},
{"����'���", 4, 2, 0, 3, Red},
{"����� �������", 5, 1, 1, 4, Green},
{"������", 6, 1, 2, 0, Red},
{"�������", 7, 0, 2, 5, NoTech},
{"�����", 8, 1, 0, 6, NoTech},
{"�����", 9, 2, 1, 7, NoTech},
{"������", 10, 1, 2, 8, Blue},
{"����", 11, 3, 0, 9, NoTech},
{"��� �����", 12, 0, 2, 8, Red},
{"�������", 13, 2, 2, 10, NoTech},
{"�����", 14, 1, 1, 11, NoTech},
{"����� �����", 15, 1, 3, 12, Blue},
{"���'���", 16, 1, 1, 2, NoTech},
{"������", 17, 1, 2, 13, NoTech},
{"����� II", 18, 2, 0, 14, Red},
{"�����", 19, 3, 1, 15, Green, WormholeAlpha},
{"����", 20, 2, 1, 0, Green, WormholeBeta},
{"����� IV", 1, 2, 3, 2, NoTech},
{"����", 2, 1, 1, 3, NoTech},
{"�����", 3, 0, 3, 5, NoTech},
{"������", 4, 0, 3, 6, Blue},
{"���������", 5, 3, 1, 7, NoTech},
{"��������", 6, 2, 0, 0, NoTech},
{"������", 7, 3, 1, 14, Blue},
{"�������", 8, 2, 1, 2, NoTech},
{"���", 9, 1, 2, 3, Red},
{"���", 10, 0, 4, 6, NoTech},
{"�����", 11, 2, 0, 0, Blue},
{"������", 12, 1, 1, 2, Green},
};
INSTELEM(planeti)
struct solar_range {
	variant_s	type;
	char		from;
	char		to;
};
static solar_range solar_range_data[] = {{Solar, 21, 24},
{AsteroidField, 25, 28},
{Nebula, 29, 30},
{Supernova, 31, 32},
};

int	planeti::getinfluence() const {
	return influence;
}

int	planeti::getresource() const {
	return resource;
}

int	planeti::getproduction() const {
	auto bonus = 2;
	auto player = getplayer();
	return getresource() + bonus;
}

int planeti::get(const playeri* player, int(planeti::*getproc)() const, unsigned flags) {
	auto result = 0;
	for(auto& e : bsdata<planeti>()) {
		if(e.getplayer() != player)
			continue;
		if((flags & Ready) != 0 && e.is(Exhaused))
			continue;
		result += (e.*getproc)();
	}
	return result;
}

void planeti::refresh() {
	for(auto& e : bsdata<planeti>())
		e.flags = 0;
}

void planeti::initialize() {
	for(auto& e : bsdata<solari>())
		e.set(Solar);
	for(auto& e : solar_range_data) {
		for(auto i = e.from; i <= e.to; i++)
			bsdata<solari>::elements[i].set(e.type);
	}
	for(auto& e : bsdata<planeti>()) {
		e.player.clear();
		e.flags = 0;
	}
}

void planeti::setup() {
	for(auto& e : bsdata<planeti>()) {
		if(!e.home)
			continue;
		e.setplayer(playeri::find(e.home));
		e.set(ObjectUsed);
	}
}

int get_system_count() {
	auto result = 0;
	for(auto e : solar_indecies) {
		if(e == Blocked)
			continue;
		result++;
	}
	return result;
}

void planeti::create_stars() {
	char player_pos[][2] = {{3, 0}, {6, 0}, {0, 3}, {6, 3}, {0, 6}, {3, 6}};
	memset(solar_indecies, 0, sizeof(solar_indecies));
	for(auto& e : bsdata<solari>()) {
		e.setplayer(0);
		e.setindex(0xFF);
	}
	for(auto y = 0; y < 7; y++) {
		if(y < 3) {
			for(auto x = 0; x < (3 - y); x++)
				solar_indecies[gmi(x, y)] = Blocked;
		} else {
			for(auto x = 7 - (y - 3); x < 8; x++)
				solar_indecies[gmi(x, y)] = Blocked;
		}
		solar_indecies[gmi(7, y)] = Blocked;
	}
	for(auto x = 0; x < 8; x++)
		solar_indecies[gmi(x, 7)] = Blocked;
	adat<int, 32> solar_deck;
	for(auto i = 1; i <= 32; i++)
		solar_deck.add(i);
	zshuffle(solar_deck.data, solar_deck.count);
	solar_indecies[gmi(3, 3)] = Blocked; // ������� ����
	for(auto e : player_pos)
		solar_indecies[gmi(e[0], e[1])] = Blocked;
	auto allowed_system = get_system_count();
	auto index = 0;
	for(auto& e : solar_indecies) {
		if(e == Blocked)
			continue;
		if(index < solar_deck.getcount())
			e = solar_deck.data[index++];
	}
	// ����������� �������� ����
	solar_indecies[gmi(3, 3)] = 0;
	// ���������� �������
	for(auto& pn : bsdata<planeti>()) {
		if(!pn.home)
			continue;
		pn.setsolar(0);
		pn.remove(ObjectUsed);
	}
	index = 33;
	int player_index = 0;
	for(auto& e : bsdata<playeri>()) {
		for(auto& pn : bsdata<planeti>()) {
			if(!pn.home)
				continue;
			if(strcmp(pn.home, e.id) == 0)
				pn.setsolar(bsdata<solari>::elements + index);
		}
		solar_indecies[gmi(player_pos[player_index][0], player_pos[player_index][1])] = index;
		player_index++;
		index++;
	}
	// ������� ������� �������� ������ 
	for(unsigned i = 0; i < sizeof(solar_indecies) / sizeof(solar_indecies[0]); i++) {
		auto s = solar_indecies[i];
		if(s == Blocked)
			continue;
		auto& e = bsdata<solari>::elements[s];
		e.setindex(i);
		for(auto& u : bsdata<planeti>()) {
			if(u.getsolar() == &e)
				u.set(ObjectUsed);
		}
	}
}

int uniti::getmovement(short unsigned index) {
	if(index == Blocked)
		return 0;
	return movement_rate[index];
}

solari* solari::getsolar(short unsigned index) {
	auto n = solar_indecies[index];
	if(n == Blocked)
		return 0;
	return bsdata<solari>::elements + n;
}

enum direction_s : unsigned char {
	LeftUp, RightUp, Left, Right, LeftDown, RightDown
};

static unsigned char getmovement(unsigned char index, direction_s d) {
	if(index == Blocked)
		return Blocked;
	auto x = gmx(index);
	auto y = gmy(index);
	switch(d) {
	case Left: x--; break;
	case Right: x++; break;
	case LeftUp: y--; break;
	case LeftDown: y++; x--; break;
	case RightUp: y--; x++; break;
	case RightDown: y++; break;
	default: return Blocked;
	}
	if(x < 0 || x >= map_scan_line || y < 0 || y >= map_scan_line)
		return Blocked;
	auto i = gmi(x, y);
	if(solar_indecies[i] == Blocked)
		return Blocked;
	return i;
}

static void make_wave(unsigned char start_index, const playeri* player, unsigned char* result, bool block) {
	static direction_s directions[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};
	unsigned char stack[256 * 8];
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	result[start_index] = 0;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = result[index] + 1;
		auto p = solari::getsolar(index);
		if(!p)
			continue;
		auto p_player = p->getplayer();
		auto allow_movement = true;
		auto type = p->getgroup();
		if(type == Nebula)
			allow_movement = false;
		else if(p_player && p_player->isenemy(player) && !p_player->is(LightWaveDeflector))
			allow_movement = false;
		if(allow_movement) {
			for(auto d : directions) {
				auto i1 = getmovement(index, d);
				if(i1 == Blocked || result[i1] == Blocked)
					continue;
				if(result[i1] < cost)
					continue;
				result[i1] = cost;
				*push_counter++ = i1;
				if(push_counter >= stack_end)
					push_counter = stack;
			}
		}
	}
}

static void make_wave(unsigned char start_index, const playeri* player) {
	for(auto& e : movement_rate)
		e = DefaultCost;
	for(auto& e : bsdata<solari>()) {
		if(!e)
			continue;
		auto type = e.getgroup();
		if(type == Solar || type == Nebula)
			continue;
		if(type == AsteroidField && player->is(AntimassDeflectors))
			continue;
		movement_rate[e.getindex()] = Blocked;
	}
	make_wave(start_index, player, movement_rate, false);
}

static int solar_caps(varianta& source) {
	auto result = 0;
	for(auto v : source) {
		auto p = v.getunit();
		if(!p)
			continue;
		result += p->getcapacity();
	}
	return result;
}

void playeri::moveground(solari* source, solari* target) {
	varianta a1, a2;
	a1.add_units(this);
	a1.match(GroundForces, Fighters);
	if(!a1)
		return;
	string sb;
	sb.add("�� ������ ��������� ��������� �������. ����� ��������� ������, ������� ����� ���������. ����� ���������, ��� ���� �������� �� ������ �����.");
	if(!choose(a1, a2, sb, "���������", true))
		return;
	for(auto pv : a2) {
		auto p = pv.getunit();
		if(!p)
			continue;
		p->setsolar(target);
	}
}

void playeri::moveships(solari* solar) {
	varianta a1, a2, a3;
	// ������ �����
	make_wave(solar->getindex(), this);
	a1.add_units(this);
	a1.match_movement(solar);
	if(!a1)
		return;
	// ����������� �������� � �������
	string sb;
	sb.add("�� ������ ����������� ��������� ����� �������� � ������� [%1]. ����� ��������� �������, ������� ����� �����������. ��� �����������, �������� �� ������� �����.", solar->getname());
	if(!choose(a1, a2, sb, "�����������", true))
		return;
	// ���������� �������� �����
	a3.add_solars(a2);
	a3.rollup();
	for(auto pv : a3)
		moveground(pv.getsolar(), solar);
	// ���������� �������
	for(auto pv : a2) {
		auto p = pv.getunit();
		if(!p)
			continue;
		p->setsolar(solar);
	}
	// ����� ��� �������� ����������� �������
	if(!solar->getplayer())
		solar->setplayer(this);
}

planeti* planeti::find(const solari* parent, int index) {
	if(!index)
		return 0;
	for(auto& e : bsdata<planeti>()) {
		if(e.getsolar() == parent) {
			if(--index == 0)
				return &e;
		}
	}
	return 0;
}

int	planeti::getcount(variant_s type, const playeri* player) const {
	auto result = 0;
	for(auto& e : bsdata<uniti>()) {
		if(!e)
			continue;
		if(e.is(type) && e.getplayer() == player && e.getplanet()==this)
			result++;
	}
	return result;
}

int	planeti::get(action_s id) const {
	switch(id) {
	case Resource: return resource;
	case Influence: return influence;
	default: return 0;
	}
}