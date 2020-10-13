#include "main.h"

string::string() : stringbuilder(buffer, buffer + sizeof(buffer) / sizeof(buffer[0])),
player(0), planet(0) {
	buffer[0] = 0;
}

void string::addidentifier(const char* identifier) {
	if(planet && strcmp(identifier, "planet") == 0)
		add("[%1]", planet->getname());
	else if(strcmp(identifier, "count") == 0)
		add("[%1i]", count);
	else
		stringbuilder::addidentifier(identifier);
}