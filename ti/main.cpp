#include "main.h"
#include "draw.h"

int main() {
	srand(clock());
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Звездная империя");
	//player_info::add(SardakkNOrr);
	player_info::add(UniversitiesOfJolNar);
	player_info::add(TheEmiratesOfHacan);
	//player_info::add(TheL1z1xMindnet);
	player_info::add(TheBaronyOfLetnev);
	player_info::add(FederationOfSol);
	player_info::setup();
	player_info::sethuman(FederationOfSol);
	player_info::gethuman()->make_move();
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