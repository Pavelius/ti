#include "main.h"

static bool politic_select(unsigned char value) {
	for(auto& e : players) {
		if(e.ingame && e.politic == value)
			return false;
	}
	return true;
}

static const char* politic_getname(unsigned char value) {
	return getstr((politic_s)value);
}

static int politic_compare(const void* p1, const void* p2) {
	return 0;
}

static bool player_select(unsigned char value) {
	return players[value].ingame;
}

//unsigned char game::choose(player_s player, unsigned char i1, unsigned char i2, const char* title,
//	bool (*select)(unsigned char index),
//	const char* (*source_getname)(unsigned char index),
//	int(*compare)(const void* p1, const void* p2)) {
//	querry source(i1, i2, select);
//	if(players[player].interactive)
//		return draw::choose(source, title, source_getname);
//	else {
//		return 0;
//	}
//}

//void game::strategic() {
//	querry source(0, LastPlayer, player_select);
//	for(auto player : source.col<player_s>()) {
//		if(!players[player].ingame)
//			continue;
//		players[player].politic = (politic_s)choose(player, Initiative, Imperial, "Укажите политику",
//			politic_select, politic_getname, politic_compare);
//	}
//}