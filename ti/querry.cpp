#include "main.h"

void querry::add(unsigned char i1, unsigned char i2, bool(*selecting)(unsigned char index)) {
	for(auto i = i1; i <= i2; i++)
		if(selecting(i))
			adat::add(i);
}

void querry::sort(int(*compare)(const void* p1, const void* p2)) {
	qsort(data, count, sizeof(data[0]), compare);
}

static int compare_player_politics(const void* p1, const void* p2) {
	auto& e1 = players[*((player_s*)p1)];
	auto& e2 = players[*((player_s*)p2)];
	return e2.getinitiative() - e1.getinitiative();
}

static int compare_politic(const void* p1, const void* p2) {
	return 0;
}