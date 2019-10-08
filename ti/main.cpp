#include "main.h"
#include "draw.h"

using namespace code;

static void test_proc(const playeri* p) {
	planeta source; p->select(source, Ready|Friendly);
	if(source)
		source[0]->set(Exhaused);
}

static void test_metadata() {
	metadata::initialize();
	auto p = metadata::addtype("Rectangle");
	p->add("x1", "Integer");
	p->add("y1", "Integer");
	p->add("x2", "Integer");
	p->add("y2", "Integer");
	p = metadata::addtype("Box");
	p->add("rect", "Rectangle*");
	p->add("points", "Point[]");
	p->write("Box.mtd");
}

int main() {
	//return 0;
	test_metadata();
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
	playeri::make_move(true, true);
	//playeri::get("norr")->add(Goods, 5);
	//playeri::get("norr")->strategy = Construction;
	//playeri::make_move(false, true);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}