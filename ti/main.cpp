#include "view.h"

extern bool		sys_optimize_mouse_move;

using namespace draw;
using namespace draw::controls;

void test_list() {
	const int pixel_per_line = 24;
	const int list_height = pixel_per_line * 4 + 1;
	army a1; a1.count = select(a1.data, endofs(a1.data), 0, TheEmiratesOfHacan);
	unitlist e1(a1);
	unitlist e2(a1);
	e1.show_grid_lines = false;
	while(ismodal()) {
		rectf({0, 0, getwidth(), getheight()}, colors::window);
		e1.view({100, 100, 300, 100 + list_height});
		e2.view({310, 100, 600, 100 + list_height});
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