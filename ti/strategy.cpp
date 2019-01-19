#include "main.h"

static struct strategy_info {
	const char*	id;
	const char*	name;
	char		initiative;
	char		bonus;
} strategy_data[] = {{"", ""},
{"Initiative", "Инициативная", 1},
{"Diplomacy", "Дипломатическая", 2},
{"Political", "Политическая", 3},
{"Logistics", "Логистическая", 4},
{"Trade", "Торговая", 5},
{"Warfare", "Военная", 6},
{"Technology", "Технологическая", 7},
{"Imperial", "Империалистическая", 8},
};
getstr_enum(strategy);

int player_info::getinitiative(strategy_s value) {
	return strategy_data[value].initiative;
}