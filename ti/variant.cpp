#include "main.h"

varianti bsmeta<varianti>::elements[] = {{""},
{"Player", "�����"},
{"Solar", "�������� �������"},
{"AsteroidField", "���� ����������"},
{"Nebula", "������"},
{"Supernova", "���������"},
{"Planet", "�������"},
{"Unit", "�������������"},
{"SpaceDock", "����", 3, 0, 1, 0, 0, 2},
{"GroundForces", "�������� ����", 0, 1, 2, 0, 2, 8},
{"Fighters", "�����������", 0, 1, 2, 0, 2, 9},
{"PDS", "���", 6, 0, 1, 0, 1, 6},
{"Carrier", "���������", 4, 3, 1, 1, 1, 9},
{"Cruiser", "�������", 8, 2, 1, 2, 1, 7},
{"Destroyer", "�������", 8, 1, 1, 2, 1, 9},
{"Dreadnought", "������", 5, 5, 1, 1, 1, 5},
{"WarSun", "������ ������", 2, 12, 1, 2, 1, 0, {3, 3}},
{"Agenda", "��������"},
{"Technology", "����������"},
{"Variant", "�������"},
};
assert_enum(variant, Variant);
