#include "view.h"

using namespace draw;
using namespace draw::controls;

class control_querry_list : public list {
	querry&	source;
	const char* (*source_getname)(unsigned char value);
	int getmaximum() const override {
		return source.count;
	}
	const char* getname(char* result, const char* result_maximum, int line, int column) const {
		return source_getname(source[line]);
	}
public:
	int getvalue() const {
		return source[current];
	}
	control_querry_list(querry& source, const char* (*source_getname)(unsigned char index)) : source(source), source_getname(source_getname) {
	}
};

struct control_player_table : public table {
	adat<player_s, 6> source;
	int getmaximum() const override {
		return source.count;
	}
	const char* getname(char* result, const char* result_maximum, int line, int column) const override {
		if(columns[column] == "name")
			return getstr(source[line]);
		else if(columns[column] == "politic")
			return getstr(players[source[line]].politic);
		return 0;
	}
	int getnumber(int line, int column) const override {
		if(columns[column] == "resource")
			return planet::get(source[line], &planet::getresource);
		else if(columns[column] == "influence")
			return planet::get(source[line], &planet::getinfluence);
		else if(columns[column] == "planet_count")
			return planet::get(source[line], &planet::getone);
		else if(columns[column] == "fleet")
			return players[source[line]].getfleet();
		else if(columns[column] == "command")
			return players[source[line]].getcommand();
		else if(columns[column] == "strategy")
			return players[source[line]].getstrategy();
		return 0;
	}
	player_s getvalue() const {
		return source[current];
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "Наименование", 160},
		{Text, "politic", "Политика", 100},
		{Number | AlignRight, "resource", "Рс", 32},
		{Number | AlignRight, "influence", "Вл", 32},
		{Number | AlignRight, "planet_count", "Пл", 32},
		{Number | AlignRight, "fleet", "Флот", 32},
		{Number | AlignRight, "command", "Ком", 32},
		{Number | AlignRight, "strategy", "Стр", 32},
		{}
		};
		return columns;
	}
	static int compare(const void* p1, const void* p2) {
		auto i1 = *((player_s*)p1);
		auto i2 = *((player_s*)p2);
		auto d = players[i1].getinitiative() - players[i2].getinitiative();
		if(d==0)
			return strcmp(getstr(i1), getstr(i2));
		return d;
	}
	void initialize() {
		source.clear();
		for(auto i = FirstPlayer; i <= LastPlayer; i = (player_s)(i + 1)) {
			if(players[i].ingame)
				source.add(i);
		}
		qsort(source.data, source.count, sizeof(source.data[0]), compare);
	}
	control_player_table() : table(getcolumns()) {
		initialize();
	}
};

class control_player_statistic : public control_player_table {
	bool isfocusable() const override {
		return false;
	}
public:
	control_player_statistic() {
		show_selection = false;
		show_border = false;
	}
};

class control_unit_production : public table {
	player_s player;
	unit units_production[WarSun - GroundForces + 1];
	char units_count[WarSun - GroundForces + 1];
	int production_limit;
	int getmaximum() const override {
		return sizeof(units_production) / sizeof(units_production[0]);
	}
	void decrease() {
		units_count[current]--;
		if(units_count[current] <= 0)
			units_count[current] = 0;
	}
	void increase() {
		units_count[current]++;
	}
	int getfocused(int line) const {
		return isfocused() && (current == line);
	}
	static const column* getcolumns() {
		static constexpr column columns[] = {{Text, "name", "Наименование", 128},
		{Number | AlignRight | TextBold, "resource", "Ресурсы", 32},
		{Number | AlignRight, "strenght", "Сила", 32},
		{Number | AlignRight, "move", "Движение", 32},
		{Number | AlignRight, "capacity", "Вместимость", 32},
		{Number | AlignRight, "order", "", 32},
		{Button | AlignRight, "order_buttons", "Заказ", 48},
		{}
		};
		return columns;
	}
	void custom(char* temp, const char* temp_maximum, rect rc, int line, int column) const override {
		auto height = rc.height();
		auto width = height;
		bool disabled;
		if(columns[column] == "order_buttons") {
			rc.x1 += 4;
			if(buttonv({rc.x1, rc.y1, rc.x1 + width, rc.y1 + height}, false, getfocused(line), units_count[line] == 0, true, "-", KeyLeft, true))
				execute((callback)&control_unit_production::decrease);
			rc.x1 += height + 4;
			disabled = production_limit && getproductionlimit() >= production_limit;
			if(buttonv({rc.x1, rc.y1, rc.x1 + width, rc.y1 + height}, false, getfocused(line), disabled, true, "+", KeyRight, true))
				execute((callback)&control_unit_production::increase);
			rc.x1 += height + 4;
		}
	}
	const char* getname(char* result, const char* result_maximum, int line, int column) const {
		if(columns[column] == "name")
			return units_production[line].getname();
		return result;
	}
	int getnumber(int line, int column) const override {
		if(columns[column] == "production")
			return units_production[line].getproduction();
		else if(columns[column] == "order")
			return getcount((unit_s)(GroundForces + line));
		else if(columns[column] == "move")
			return units_production[line].getmovement();
		else if(columns[column] == "capacity")
			return units_production[line].getcapacity();
		else if(columns[column] == "resource")
			return units_production[line].getresource();
		else if(columns[column] == "strenght")
			return units_production[line].getstrenght();
		else if(columns[column] == "hits")
			return units_production[line].getmaxhits();
		return 0;
	}
	const char* gettotal(char* result, const char* result_maximum, int column) const override {
		if(columns[column] == "name")
			return "Итого:";
		return 0;
	}
	int getproductionlimit() const {
		auto result = 0;
		for(auto i = GroundForces; i <= WarSun; i = (unit_s)(i + 1))
			result += getcount(i);
		return result;
	}
	int gettotal(int column) const override {
		if(columns[column] == "order") {
			return getproductionlimit();
		} else if(columns[column] == "resource") {
			auto result = 0;
			for(auto& e : units_production)
				result += units_count[&e - units_production] * e.getresource();
			return result;
		}
		return 0;
	}
public:
	control_unit_production(player_s player, int production_limit) : table(getcolumns()), player(player), production_limit(production_limit) {
		for(auto i = GroundForces; i <= WarSun; i = (unit_s)(i + 1)) {
			units_production[i - GroundForces] = unit(i, 0, player);
			units_count[i - GroundForces] = 0;
		}
		show_totals = true;
	}
	int get(unit_s id) const {
		if(id >= GroundForces && id <= WarSun)
			return units_count[id - GroundForces];
		else
			return 0;
	}
	int getcount(unit_s id) const {
		return units_count[id - GroundForces] * units_production[id - GroundForces].getproduction();
	}
};

class control_planets : table {

	column* getcolumns() const {
		return 0;
	}

public:
	control_planets() : table(getcolumns()) {}
};

static bool info_point(int x, int& y) {
	rect rc = {x - 16, y - 16, x + 16, y + 16};
	draw::circlef(x, y, 16, colors::form, 128);
	draw::circle(x, y, 16);
	return areb(rc);
}

static void show_statistic() {
	rect rc;
	control_player_statistic mv;
	mv.current = -1;
	rc.x1 = getwidth() - 780;
	rc.y1 = gui_data.border * 2;
	rc.x2 = getwidth() - 32 - gui_data.border * 3;
	rc.y2 = rc.y1 + mv.getrowheight() * (mv.source.getcount() + 1);
	draw::window(rc, false, false);
	mv.view(rc);
}

static void show_right_buttoms() {
	auto x = getwidth() - 16 - gui_data.border;
	auto y = 16 + gui_data.border;
	if(info_point(x, y))
		show_statistic();
}

bool draw::production(player_s player, int production_limit) {
	control_unit_production mv(player, production_limit);
	setfocus(0, true);
	while(ismodal()) {
		const int production_width = 400;
		const int table_height = 10;
		board();
		auto rc = window("Производство");
		mv.view(rc);
		show_right_buttoms();
		auto id = input();
		if(!boardkeys(id))
			defproc(id);
	}
	return true;
}

unsigned char draw::choose(querry& source, const char* title, const char* (*source_getname)(unsigned char index)) {
	control_querry_list mv(source, source_getname);
	setfocus(0, true);
	while(ismodal()) {
		board();
		auto rc = window(title, AcceptButton);
		if(true) {
			draw::state push;
			draw::font = metrics::h3;
			mv.view(rc);
		}
		show_right_buttoms();
		icon(200, 200, GroundForces, 2);
		icon(230, 200, PDS, 1);
		icon(270, 200, Fighters, 1);
		icon(310, 200, Carrier, 1);
		auto id = input();
		switch(id) {
		case KeyEnter:
			breakmodal(AcceptButton);
			break;
		}
		if(defproc(id))
			continue;
		if(boardkeys(id))
			continue;
	}
	if(getresult())
		return mv.getvalue();
	return 0;
}