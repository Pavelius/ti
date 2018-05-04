#include "main.h"
#include "view.h"

player_s game::randomplayer() {
	gameplayers players;
	if(players.count == 0)
		return NoPlayer;
	return players.data[rand() % players.count];
}

void game::strategic() {
	gameplayers selections;
	for(auto player : selections) {
		players[player].politic = draw::choosepolitic();
	}
}