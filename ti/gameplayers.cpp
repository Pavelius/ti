#include "main.h"

static int compare(const void* p1, const void* p2) {
	auto& e1 = players[*((player_s*)p1)];
	auto& e2 = players[*((player_s*)p2)];
	return e2.getinitiative() - e1.getinitiative();
}

gameplayers::gameplayers() {
	for(auto i = FirstPlayer; i <= LastPlayer; i = (player_s)(i + 1)) {
		if(!players[i].ingame)
			continue;
		add(i);
	}
	qsort(data, count, sizeof(data[0]), compare);
}

static player_s left(player_s value) {
	if(value == FirstPlayer)
		return LastPlayer;
	return (player_s)(value - 1);
}

static player_s right(player_s value) {
	if(value == LastPlayer)
		return FirstPlayer;
	return (player_s)(value + 1);
}

gameplayers::gameplayers(player_s speaker) {
	auto stop = speaker;
	do {
		if(players[speaker].ingame)
			add();
		speaker = right(speaker);
	} while(speaker != stop);
}