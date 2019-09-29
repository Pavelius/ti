#include "main.h"

void costi::add(action_s id, int v) {
	actions[id] += v;
	if(actions[id] < 0)
		actions[id] = 0;
}

void costi::difference(string& sb, const costi& e) {
}