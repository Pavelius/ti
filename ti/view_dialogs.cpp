#include "view.h"

using namespace draw;
using namespace draw::controls;

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
		{Number | AlignRight, "production", "Количество", 32},
		{Number | AlignRight, "strenght", "Сила", 32},
		{Number | AlignRight, "move", "Движение", 32},
		{Number | AlignRight, "hits", "Хиты", 32},
		{Number | AlignRight, "capacity", "Вместимость", 32},
		{Number | AlignRight, "order", "Заказ", 32},
		{Button | AlignRight, "order_buttons", "", 28},
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

bool draw::dialogs::production(int production_limit) {
	control_unit_production e2(SardakkNOrr, production_limit);
	const int pixel_per_line = 24;
	while(ismodal()) {
		const int production_width = 400;
		const int table_height = 9;
		board();
		auto x = 20, y = 20;
		window({x, y, x + production_width + 200, y + pixel_per_line * table_height + 25});
		e2.view({x, y, x + production_width, y + pixel_per_line * table_height + 1});
		x += production_width + 10;
		//e1.view({x, y, x + 120, y + pixel_per_line * table_height + 1});
		auto id = input();
		defproc(id);
	}
	return true;
}