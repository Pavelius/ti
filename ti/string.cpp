#include "main.h"

string::string() : stringbuilder(buffer, buffer + sizeof(buffer) / sizeof(buffer[0])),
player(0) {
	buffer[0] = 0;
}

void string::addidentifier(const char* identifier) {
	stringbuilder::addidentifier(identifier);
}