#include "main.h"

static struct politic_info {
	const char*	id;
	const char*	name;
	char		initiative;
} politic_data[] = {{"", ""},
{"Initiative", "������������", 1},
{"Diplomacy", "���������������", 2},
{"Political", "������������", 3},
{"Logistics", "�������������", 4},
{"Trade", "��������", 5},
{"Warfare", "�������", 6},
{"Technology", "���������������", 7},
{"Imperial", "������������������", 8},
};
getstr_enum(politic);

int player::getinitiative(politic_s value) {
	return politic_data[value].initiative;
}