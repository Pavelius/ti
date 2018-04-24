#include "view.h"

extern bool		sys_optimize_mouse_move;

using namespace draw;
using namespace draw::controls;

void test_list() {
	constexpr static column planets_columns[] = {{"Наименование", &unit::getname},
	{"Цена", &unit::getresource},
	{}
	};
	constexpr static column production_columns[] = {{"Наименование", &unit::getname},
	{"Ресурсы", &unit::getresource},
	{"Количество", &unit::getproduction},
	{}
	};
	adat<unit, 16> production_unit;
	for(auto t1 = GroundForces; t1 <= WarSun; t1 = (unit_s)(t1 + 1))
		production_unit.add(t1);
	army t1;
	for(auto& e : production_unit)
		t1.add(&e);
	const int pixel_per_line = 24;
	army a1; a1.count = select(a1.data, endofs(a1.data), 0, TheEmiratesOfHacan);
	unitlist e1(a1, planets_columns);
	unitlist e2(t1, production_columns);
	e1.show_grid_lines = false;
	while(ismodal()) {
		rectf({0, 0, getwidth(), getheight()}, colors::window);
		e2.view({100, 100, 300, 100 + pixel_per_line * 8 + 1});
		e1.view({310, 100, 600, 100 + pixel_per_line * 5});
		input();
	}
}

int main() {
	sys_optimize_mouse_move = true;
	srand(clock());
	players[TheEmiratesOfHacan].initialize();
	players[TheL1z1xMindnet].initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Twilight Imperium 3ed");
	test_list();
	//draw::report("Тестовая строка окна, которое появился при выводе сообщения.\n$(accept)");
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}