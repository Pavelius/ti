#include "main.h"

strategyi bsmeta<strategyi>::elements[] = {{"", ""},
{"Initiative", "������������", "", 1},
{"Diplomacy", "���������������", "�� ���������� ������������ ��������������� � ����� ������. �� ��, �� ��, �� ������� �������� ���� �� �����.", 2},
{"Political", "������������", "�� ���������� ����� �� ����������� �� ������ ������ ���� ���. ����� ������������ �� ����������� ����� �� �� ���������� ��� ���. ����� ����� �� ��������� [3] ����� �������� � [1] ��������� ����.", 3},
{"Logistics", "�������������", "", 4},
{"Trade", "��������", "", 5},
{"Warfare", "�������", "", 6},
{"Technology", "���������������", "", 7},
{"Imperial", "������������������", "", 8},
};
assert_enum(strategy, Imperial);

int playeri::getinitiative(strategy_s value) {
	return bsmeta<strategyi>::elements[value].initiative;
}