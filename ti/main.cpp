#include "view.h"

extern bool	sys_optimize_mouse_move;

using namespace draw;
using namespace draw::controls;

void test_list() {
	constexpr static unitlist::column planets_columns[] = {{"Наименование", &unit::getname},
	{"Цена", &unit::getresource},
	{}
	};
	constexpr static unitlist::column production_columns[] = {{"Наименование", &unit::getname},
	{"Ресурсы", &unit::getresource, AlignRight | TextBold},
	{"Количество", &unit::getproduction},
	{"Сила", &unit::getstrenght},
	{"Движение", &unit::getmovement},
	{"Хиты", &unit::getmaxhits},
	{"Вместимость", &unit::getcapacity},
	{}
	};
	adat<unit, WarSun - GroundForces + 1> production_unit;
	for(auto t1 = GroundForces; t1 <= WarSun; t1 = (unit_s)(t1 + 1))
		production_unit.add({t1, 0, TheL1z1xMindnet});
	army t1;
	for(auto& e : production_unit)
		t1.add(&e);
	const int pixel_per_line = 24;
	army a1; a1.count = select(a1.data, endofs(a1.data), 0, TheEmiratesOfHacan);
	unitlist e1(a1, planets_columns);
	unitlist e2(t1, production_columns);
	e1.show_grid_lines = false;
	while(ismodal()) {
		const int production_width = 400;
		const int table_height = 8;
		rectf({0, 0, getwidth(), getheight()}, colors::window);
		auto x = 10, y = 10;
		e2.view({x, y, x + production_width, y + pixel_per_line * table_height + 1});
		x += production_width + 10;
		e1.view({x, y, x + 120, y + pixel_per_line * table_height + 1});
		auto id = input();
		defproc(id);
	}
}

void test_hexagonal() {
	point hex_indecies[] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}};
	while(ismodal()) {
		rectf({0, 0, getwidth(), getheight()}, colors::window);
		for(auto h : hex_indecies) {
			auto pt = h2p(h);
			pt = pt + 260;
			hexagon(pt);
			char temp[64];
			szprints(temp, endofs(temp), "%1i, %2i", h.x, h.y);
			text(pt.x, pt.y, temp);
		}
		auto id = input();
		defproc(id);
	}
}

int main() {
	sys_optimize_mouse_move = true;
	srand(clock());
	players[TheEmiratesOfHacan].initialize();
	players[TheL1z1xMindnet].initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Twilight Imperium 3ed");
	//test_list();
	test_hexagonal();
	//draw::report("Тестовая строка окна, которое появился при выводе сообщения.\n$(accept)");
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}