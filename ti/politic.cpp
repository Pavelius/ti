#include "main.h"

static struct politic_info {
	const char*	id;
	const char*	name;
	char		initiative;
} politic_data[] = {{"", ""},
{"Initiative", "Инициативная", 1},
{"Diplomacy", "Дипломатическая", 2},
{"Political", "Политическая", 3},
{"Logistics", "Логистическая", 4},
{"Trade", "Торговая", 5},
{"Warfare", "Военная", 6},
{"Technology", "Технологическая", 7},
{"Imperial", "Империалистическая", 8},
};
getstr_enum(politic);

int player::getinitiative(politic_s value) {
	return politic_data[value].initiative;
}