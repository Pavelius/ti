#include "draw.h"

using namespace draw::controls;

static control*	current_hilite;
static control*	current_focus;
static control*	current_command;

static struct control_plugin : draw::renderplugin {

	void before() override {
		current_hilite = 0;
		current_focus = 0;
		current_command = 0;
	}

	bool translate(int id) override {
		if(current_focus) {
			switch(id) {
			case KeyEnter: current_focus->keyenter(); return true;
			case KeyUp: current_focus->keyup(); return true;
			case KeyDown: current_focus->keydown(); return true;
			case KeyLeft: current_focus->keyleft(); return true;
			case KeyRight: current_focus->keyright(); return true;
			case KeyHome: current_focus->keyhome(); return true;
			case KeyEnd: current_focus->keyend(); return true;
			case KeyPageUp: current_focus->keypageup(); return true;
			case KeyPageDown: current_focus->keypagedown(); return true;
			}
		}
		if(current_hilite) {
			switch(id) {
			case MouseLeft: current_hilite->mouseleft(hot::mouse); return true;
			case MouseLeftDBL: current_hilite->mouseleftdbl(hot::mouse); return true;
			case MouseWheelUp: current_hilite->mousewheel(hot::mouse, -1); return true;
			case MouseWheelDown: current_hilite->mousewheel(hot::mouse, 1); return true;
			}
		}
		return false;
	}

} control_plugin_instance;

control::control() : show_border(true) {

}

bool control::ishilited() const {
	return current_hilite == this;
}

bool control::isfocused() const {
	return current_focus == this;
}

void control::view(rect rc) {
	if(isfocusable()) {
		addelement((int)this, rc);
		if(!getfocus())
			setfocus((int)this, true);
	}
	if((control*)getfocus() == this)
		current_focus = this;
	if(areb(rc))
		current_hilite = this;
	if(show_border)
		rectb(rc, colors::border);
}