#include "main.h"

static struct politic {
	const char*	id;
	const char*	name;
	char		initiative;
} politic_data[] = {{"No Politics", "Нет политики"},
{"Initiative", "Инициативная"},
{"Diplomacy", "Дипломатическая"},
{"Political", "Политическая"},
{"Logistics", "Логистическая"},
{"Trade", "Торговая"},
{"Warfare", "Военная"},
{"Technology", "Технологическая"},
{"Imperial", "Империалистическая"},
};
getstr_enum(politic);

int player::getinitiative(politic_s value) {
	return politic_data[value].initiative;
}