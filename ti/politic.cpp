#include "main.h"

static struct politic {
	const char*	id;
	const char*	name;
	char		initiative;
} politic_data[] = {{"No Politics", "��� ��������"},
{"Initiative", "������������"},
{"Diplomacy", "���������������"},
{"Political", "������������"},
{"Logistics", "�������������"},
{"Trade", "��������"},
{"Warfare", "�������"},
{"Technology", "���������������"},
{"Imperial", "������������������"},
};
getstr_enum(politic);

int player::getinitiative(politic_s value) {
	return politic_data[value].initiative;
}