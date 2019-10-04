#include "main.h"

varianti bsmeta<varianti>::elements[] = {{""},
{"Player", "Игрок"},
{"Solar", "Звездная система"},
{"AsteroidField", "Поле астероидов"},
{"Nebula", "Небула"},
{"Supernova", "Супернова"},
{"Planet", "Планета"},
{"Unit", "Подразделение"},
{"SpaceDock", "Доки", 3, 0, 1, 0, 0, 2},
{"GroundForces", "Наземные силы", 0, 1, 2, 0, 2, 8},
{"Fighters", "Истребители", 0, 1, 2, 0, 2, 9},
{"PDS", "СПЗ", 6, 0, 1, 0, 1, 6},
{"Carrier", "Транспорт", 4, 3, 1, 1, 1, 9},
{"Cruiser", "Крейсер", 8, 2, 1, 2, 1, 7},
{"Destroyer", "Эсминец", 8, 1, 1, 2, 1, 9},
{"Dreadnought", "Линкор", 5, 5, 1, 1, 1, 5},
{"WarSun", "Звезда смерти", 2, 12, 1, 2, 1, 0, {3, 3}},
{"Agenda", "Повестка"},
{"Technology", "Технология"},
{"Variant", "Вариант"},
};
assert_enum(variant, Variant);
