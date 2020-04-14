#include "main.h"

INSTDATA(varianti) = {{""},
{"Player", "�����"},
{"Solar", "�������� �������"},
{"AsteroidField", "���� ����������"},
{"Nebula", "������"},
{"Supernova", "���������"},
{"Planet", "�������"},
{"Unit", "�������������"},
{"SpaceDock", "����", 4, 0, 1},
{"PDS", "���", 6, 0, 1, 0, 6},
{"GroundForces", "�������� ����", 0, 1, 2, 2, 8},
{"Fighters", "�����������", 0, 1, 2, 2, 9},
{"Carrier", "���������", 4, 3, 1, 1, 9},
{"Cruiser", "�������", 8, 2, 1, 2, 7},
{"Destroyer", "�������", 8, 1, 1, 2, 9},
{"Dreadnought", "������", 5, 5, 1, 1, 5},
{"WarSun", "������ ������", 2, 12, 1, 2, {3, 3}},
{"Agenda", "��������"},
{"Technology", "����������"},
{"Variant", "�������"},
};
assert_enum(variant, Variant)

int	variant::getweight() const {
	if(type==Unit) {
		auto p = getunit();
		if(p)
			return p->getweight();
	}
	return 0;
}

void variant::destroy() {
	if(type == Unit) {
		auto p = getunit();
		if(p)
			p->destroy();
	} else
		clear();
}

const char* variant::getname() const {
	switch(type) {
	case Planet: return bsdata<planeti>::elements[value].getname();
	case Solar: return bsdata<solari>::elements[value].getname();
	case Unit: return bsdata<uniti>::elements[value].getname();
	default: return "��� �����";
	}
}