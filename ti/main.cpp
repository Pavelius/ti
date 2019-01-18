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
	draw::setcaption("Звездная империя");
	player_info::make_move();
	//auto player = draw::chooseplayer();
	//game::strategic();
	//draw::production(player, 5);
	//draw::board();
	//draw::report("Тестовая строка окна, которое появился при выводе сообщения.\n$(accept)");
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}