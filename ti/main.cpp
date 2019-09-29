#include "main.h"
#include "draw.h"

int main() {
	srand(1231);
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Звездная империя");
	planeti::initialize();
	bsmeta<playeri>::add()->create("jelnar");
	bsmeta<playeri>::add()->create("hacan");
	bsmeta<playeri>::add()->create("barony");
	bsmeta<playeri>::add()->create("sol").sethuman();
	bsmeta<playeri>::add()->create("norr");
	bsmeta<playeri>::add()->create("xxcha");
	planeti::create_stars();
	playeri::setup();
	playeri::gethuman()->tactical_action();
	playeri::gethuman()->make_move();
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}