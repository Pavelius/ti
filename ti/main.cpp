#include "view.h"

extern bool	sys_optimize_mouse_move;

using namespace draw;
using namespace draw::controls;

int main() {
	sys_optimize_mouse_move = true;
	srand(clock());
	players[TheEmiratesOfHacan].initialize();
	players[TheL1z1xMindnet].initialize();
	players[TheL1z1xMindnet].interactive = true;
	players[TheBaronyOfLetnev].initialize();
	players[TheBaronyOfLetnev].interactive = true;
	players[FederationOfSol].initialize();
	players[FederationOfSol].interactive = true;
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Twilight Imperium 3ed");
	//auto player = draw::chooseplayer();
	game::strategic();
	//draw::production(player, 5);
	//draw::board();
	//draw::report("�������� ������ ����, ������� �������� ��� ������ ���������.\n$(accept)");
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}