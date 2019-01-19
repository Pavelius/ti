#include "main.h"

bsreq string_type[] = {
	BSREQ(string, player, player_type),
	BSREQ(string, opponent, player_type),
{}};

string::string() : stringcreator(buffer, buffer + sizeof(buffer) / sizeof(buffer[0])),
player(0), opponent(0) {
	buffer[0] = 0;
}

void string::addidentifier(const char* identifier) {
	stringcreator::addidentifier(identifier);
}

bool string::isplayer() const {
	if(!player)
		return false;
	return !player->iscomputer();
}