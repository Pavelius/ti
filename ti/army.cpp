#include "main.h"

void army::sort(int (unit::*proc)() const) {
	const auto i2 = getmaximum() - 1;
	const auto i1 = 0;
	for(int i = i2; i > i1; i--) {
		for(int j = i1; j < i; j++) {
			auto t1 = (data[j]->*proc)();
			auto t2 = (data[j + 1]->*proc)();
			if(t1 > t2)
				iswap(data[j], data[j + 1]);
		}
	}
}

void army::removecasualty(player_s player) {
	if(players[player].interactive) {

	} else {
		army temp;
		for(auto p : *this) {
			if(!*p)
				continue;
			temp.add(p);
		}
		temp.sort(&unit::getweight);
		if(temp.data[0])
			temp.data[0]->destroy();
	}
}