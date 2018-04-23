#include "crt.h"
#include "draw.h"

static int		current_command;
static int		current_focus;
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

static void setfocus_callback() {
	current_focus = hot::param;
}

void draw::setfocus(int id, bool instant) {
	if(instant)
		current_focus = id;
	else {
		hot::param = id;
		execute(setfocus_callback);
	}
}

int draw::getfocus() {
	return current_focus;
}

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
		for(auto p = renderplugin::first; p; p = p->next) {
			if(p->translate(hot::key))
				break;
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
	for(auto p = renderplugin::first; p; p = p->next) {
		if(p->translate(hot::key))
			break;
	}
	return hot::key;
}