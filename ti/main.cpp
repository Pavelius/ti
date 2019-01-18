#include "main.h"
#include "draw.h"

int main() {
	srand(clock());
	players[TheEmiratesOfHacan].initialize();
	players[TheL1z1xMindnet].initialize();
	players[TheL1z1xMindnet].interactive = true;
	players[TheBaronyOfLetnev].initialize();
	players[TheBaronyOfLetnev].interactive = true;
	players[FederationOfSol].initialize();
	players[FederationOfSol].interactive = true;
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("�������� �������");
	answer_info ai;
	ai.add(0, "Test");
	ai.add(1, "Second");
	ai.choose(true);
	//auto player = draw::chooseplayer();
	//game::strategic();
	//draw::production(player, 5);
	//draw::board();
	//draw::report("�������� ������ ����, ������� �������� ��� ������ ���������.\n$(accept)");
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}