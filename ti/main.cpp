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
	draw::add(-1, -1, 800, 600, WFResize | WFMinmax, 32);
	draw::setcaption("Звездная империя");
	planeti::initialize();
	auto p1 = bsdata<playeri>::add()->create("sol");
	bsdata<playeri>::add()->create("jelnar");
	bsdata<playeri>::add()->create("hacan");
	bsdata<playeri>::add()->create("barony");
	bsdata<playeri>::add()->create("norr");
	bsdata<playeri>::add()->create("xxcha");
	planeti::create_stars();
	playeri::setup();
	p1->sethuman();
	if(true)
		playeri::make_move(true, true);
	else {
		p1->add(Goods, 5);
		p1->strategy = Construction;
		p1->make_move(false, true);
	}
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}