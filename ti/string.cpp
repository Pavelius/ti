#include "main.h"

bsreq string_type[] = {
	BSREQ(string, player, player_type),
{}};

string::string() : stringcreator(buffer, buffer + sizeof(buffer) / sizeof(buffer[0])),
player(0) {
	buffer[0] = 0;
}

void string::addidentifier(const char* identifier) {
	stringcreator::addidentifier(identifier);
}