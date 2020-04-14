#include "main.h"

INSTDATA(varianti) = {{""},
{"Player", "Игрок"},
{"Solar", "Звездная система"},
{"AsteroidField", "Поле астероидов"},
{"Nebula", "Небула"},
{"Supernova", "Супернова"},
{"Planet", "Планета"},
{"Unit", "Подразделение"},
{"SpaceDock", "Доки", 4, 0, 1},
{"PDS", "СПЗ", 6, 0, 1, 0, 6},
{"GroundForces", "Наземные силы", 0, 1, 2, 2, 8},
{"Fighters", "Истребители", 0, 1, 2, 2, 9},
{"Carrier", "Транспорт", 4, 3, 1, 1, 9},
{"Cruiser", "Крейсер", 8, 2, 1, 2, 7},
{"Destroyer", "Эсминец", 8, 1, 1, 2, 9},
{"Dreadnought", "Линкор", 5, 5, 1, 1, 5},
{"WarSun", "Звезда смерти", 2, 12, 1, 2, {3, 3}},
{"Agenda", "Повестка"},
{"Technology", "Технология"},
{"Variant", "Вариант"},
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
	default: return "Нет имени";
	}
}