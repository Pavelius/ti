#include "main.h"

string::string() : stringcreator(buffer, buffer + sizeof(buffer) / sizeof(buffer[0])) {}

void string::addidentifier(const char* identifier) {
	stringcreator::addidentifier(identifier);
}