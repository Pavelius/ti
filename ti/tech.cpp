#include "main.h"

techi bsmeta<techi>::elements[] = {{"PlasmaScoring", "���������� ������", Red},
{"MagenDefenceGrid", "��������� �������� �����", Red, {1}},
{"DuraniumArmor", "���������� �����", Red, {2}},
{"AssaultCannon", "������� �����", Red, {3}},
//
{"NeuralMotivator", "���������� ���������", Green},
{"DacxiveAnimators", "���������� ��������", Green, {0, 1}},
{"HyperMetabolism", "����� ����������", Green, {0, 2}},
{"X89BacterialWeapon", "�89 ������������� ������", Green, {0, 3}},
//
{"AntimassDeflectors", "����-�������� ����������", Blue},
{"GravityDriver", "�������������� ���������", Blue, {0, 0, 1}},
{"FleetLogistics", "���������� ��������", Blue, {0, 0, 2}},
{"LightWaveDeflector", "������������� ����������", Blue, {0, 0, 3}},
};
assert_enum(tech, LastTech);