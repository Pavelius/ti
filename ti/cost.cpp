#include "main.h"

void cost_info::add(action_s id, int v) {
	actions[id] += v;
	if(actions[id] < 0)
		actions[id] = 0;
}

void cost_info::initialize() {
	actions[Fleet] = 3;
	actions[Command] = 3;
	actions[Strategy] = 2;
	actions[Goods] = 0;
}