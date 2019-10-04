#include "main.h"

agendai bsmeta<agendai>::elements[] = {{"ArchivedSecret", "��������� ������", Player},
{"ArmsReduction", "�����������"},
{"ColonialRedistribution", "����������������� �������", Planet | NoMekatol | NoHome},
{"CompensatedDisarmament", "���������������� �����������", Planet},
{"EconomicEquality", "������������� ���������", 0},
{"IncentiveProgram", "������������� ���������", 0},
{"IxthianArtifact", "����������� ��������", 0},
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
				sb.adds("��������� ��� ������, ������� ������ �������� �� ����������� ����� ��� [%1] � [%2].",
					bsmeta<agendai>::elements[data[0]].name,
					bsmeta<agendai>::elements[data[1]].name);
				sb.adds("�������� ���������� �� � ��������� �������?");
			} else if(data.count>=1) {
				sb.adds("��������� �����, ������� ����� ������ ��������� �� ����������� ��� [%1].",
					bsmeta<agendai>::elements[data[0]].name);
				sb.adds("��� ��� � ���� ������?");
			} else
				sb.adds("������, ������� ����� ��������������� ������� �� ���� ������ ����� ������ ��������������.");
			if(data.count > 1)
				ai.add(1, "�������� ������ �������");
			for(unsigned i = 0; i < data.count; i++)
				ai.add(4 + i, "�������� ����������� %1", bsmeta<agendai>::elements[data[i]].name);
			ai.add(10, "��� ���������");
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