#include "main.h"
#include "draw.h"

static int test_variant() {
	static variant s1[] = {bsmeta<playeri>::elements + 2, GroundForces};
	auto result = 0;
	for(auto& e : s1) {
		switch(e.type) {
		case Player:
			result++;
			break;
		}
	}
	return result;
}

int main() {
	return 0;
	srand(1231);
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Звездная империя");
	planeti::initialize();
	bsmeta<playeri>::add()->create("jelnar");
	bsmeta<playeri>::add()->create("hacan");
	bsmeta<playeri>::add()->create("barony");
	bsmeta<playeri>::add()->create("sol");
	bsmeta<playeri>::add()->create("norr").sethuman();
	bsmeta<playeri>::add()->create("xxcha");
	planeti::create_stars();
	playeri::setup();
	//playeri::gethuman()->tactical_action();
	//playeri::gethuman()->make_move();
	playeri::gethuman()->strategy = Leadership;
	playeri::gethuman()->make_move(false, true);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}