#include "main.h"

agendai bsmeta<agendai>::elements[] = {{"ArchivedSecret", "Утерянный секрет", Player},
{"ArmsReduction", "Разоружение"},
{"ColonialRedistribution", "Перераспределение колоний", Planet | NoMekatol | NoHome},
{"CompensatedDisarmament", "Компенсированное разоружение", Planet},
{"EconomicEquality", "Экономическое равенство", 0},
{"IncentiveProgram", "Стимулирующая программа", 0},
{"IxthianArtifact", "Исктианский артифакт", 0},
};
unsigned bsmeta<agendai>::count = sizeof(bsmeta<agendai>::elements) / sizeof(bsmeta<agendai>::elements[0]);

static abstract_deck agenda_deck;

void playeri::create_agenda_deck() {
	agenda_deck.clear();
	for(unsigned i = 0; i < bsmeta<agendai>::count; i++) {
		auto m = bsmeta<agendai>::elements[i].count;
		if(!m)
			m = 1;
		for(unsigned j = 0; j < m; j++)
			agenda_deck.add(i);
	}
	agenda_deck.shuffle();
}

void playeri::predict_next_political_card(int value) {
	adat<unsigned char, 8> data;
	for(auto i = 0; i < value; i++)
		data[i] = agenda_deck.draw();
	data.count = value;
	if(iscomputer()) {

	} else {
		auto all_coorrect = false;
		while(!all_coorrect) {
			string sb; answeri ai;
			if(data.count >= 2) {
				sb.adds("Следующие два закона, которые просит поставит на голосование сенат это [%1] и [%2].",
					bsmeta<agendai>::elements[data[0]].name,
					bsmeta<agendai>::elements[data[1]].name);
				sb.adds("Поставим голосовать их в указанном порядке?");
			} else if(data.count>=1) {
				sb.adds("Следующий закон, который сенат просит поставить на голосование это [%1].",
					bsmeta<agendai>::elements[data[0]].name);
				sb.adds("Что нам с этим делать?");
			} else
				sb.adds("Законы, которые будут рассматриваться сенатом на этой сессии будут полной неожиданностью.");
			if(data.count > 1)
				ai.add(1, "Поменять законы местами");
			for(unsigned i = 0; i < data.count; i++)
				ai.add(4 + i, "Отменить голосование %1", bsmeta<agendai>::elements[data[i]].name);
			ai.add(10, "Все корректно");
			auto i = choose(ai, false, sb);
			switch(i) {
			case 1:
				iswap(data[0], data[1]);
				break;
			case 4: case 5: case 6: case 7:
				agenda_deck.discard(data[i-4]); data.remove(i - 4);
				break;
			case 10:
				all_coorrect = true;
				break;
			}
		}
	}
	for(int i = data.count - 1; i >= 0; i--)
		agenda_deck.top(data[i]);
}