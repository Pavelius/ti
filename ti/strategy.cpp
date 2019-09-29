#include "main.h"

strategyi bsmeta<strategyi>::elements[] = {{"", ""},
{"Initiative", "Инициативная", "", 1},
{"Diplomacy", "Дипломатическая", "Вы достигаете определенных договоренностей с вашим врагом. Не он, не вы, не сможете нападать друг на друга.", 2},
{"Political", "Политическая", "Вы выдвигаете закон на голосование на съезде послов всех рас. Общим голосованием вы определяете будет ли он дейсвовать или нет. Кроме этого вы получаете [3] карты действия и [1] командное очко.", 3},
{"Logistics", "Логистическая", "", 4},
{"Trade", "Торговая", "", 5},
{"Warfare", "Военная", "", 6},
{"Technology", "Технологическая", "", 7},
{"Imperial", "Империалистическая", "", 8},
};
assert_enum(strategy, Imperial);

int playeri::getinitiative(strategy_s value) {
	return bsmeta<strategyi>::elements[value].initiative;
}