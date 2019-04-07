#include "main.h"
#include "draw.h"

int main() {
	srand((int)time(0));
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Звездная империя");
	planet_info::initialize();
	players[0].create("jelnar");
	players[1].create("hacan");
	players[2].create("barony");
	players[3].create("sol"); players[3].sethuman();
	players[4].create("norr");
	players[5].create("xxcha");
	planet_info::create_stars();
	player_info::setup();
	player_info::gethuman()->tactical_action();
	player_info::gethuman()->make_move();
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}