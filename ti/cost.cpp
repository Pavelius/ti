#include "main.h"

void cost_info::add(action_s id, int v) {
	actions[id] += v;
	if(actions[id] < 0)
		actions[id] = 0;
}

void cost_info::difference(string& sb, const cost_info& e) {
}