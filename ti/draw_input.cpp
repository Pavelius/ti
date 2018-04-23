#include "crt.h"
#include "draw.h"

static int		current_command;
static void		(*current_execute)();
extern rect		sys_static_area;

static struct input_plugin : draw::renderplugin {

	void before() override {
		current_command = 0;
		current_execute = 0;
		hot::cursor = CursorArrow;
		if(hot::mouse.x < 0 || hot::mouse.y < 0)
			sys_static_area.clear();
		else
			sys_static_area = {0, 0, draw::getwidth(), draw::getheight()};
	}

	//void after() override {
	//	draw::rectf(sys_static_area, colors::blue, 128);
	//}

} input_plugin_instance;

void draw::execute(int id, int param) {
	current_command = id;
	hot::key = 0;
	hot::param = param;
}

void draw::execute(void(*proc)()) {
	execute(InputExecute, 0);
	current_execute = proc;
}

int draw::input(bool redraw) {
	if(current_command) {
		hot::key = current_command;
		if(current_execute) {
			current_execute();
			hot::key = InputUpdate;
		}
		return hot::key;
	}
	// After render plugin events
	for(auto p = renderplugin::first; p; p = p->next)
		p->after();
	hot::key = InputUpdate;
	if(redraw)
		draw::sysredraw();
	else
		hot::key = draw::rawinput();
	if(!hot::key)
		exit(0);
	return hot::key;
}