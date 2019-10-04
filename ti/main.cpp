#include "main.h"
#include "draw.h"

static void test_proc(const playeri* p) {
	planeta source; p->select(source, Ready|Friendly);
	if(source)
		source[0]->set(Exhaused);
}

int main() {
	//return 0;
	srand(1231);
	draw::initialize();
	draw::create(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("�������� �������");
	planeti::initialize();
	bsmeta<playeri>::add()->create("jelnar");
	bsmeta<playeri>::add()->create("hacan");
	bsmeta<playeri>::add()->create("barony");
	bsmeta<playeri>::add()->create("sol");
	bsmeta<playeri>::add()->create("norr").sethuman();
	bsmeta<playeri>::add()->create("xxcha");
	planeti::create_stars();
	playeri::setup();
	//test_proc(playeri::get("barony"));
	playeri::make_move(true, true);
	//playeri::get("norr")->strategy = Technology;
	//playeri::get("norr")->add(Goods, 5);
	//playeri::make_move(false, true);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}