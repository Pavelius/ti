#include "main.h"

static int compare(const void* p1, const void* p2) {
	auto e1 = (answer_info::element*)p1;
	auto e2 = (answer_info::element*)p2;
	return strcmp(e1->text, e2->text);
}

answer_info::answer_info() : stringcreator(buffer, zendof(buffer)) {}

void answer_info::sort() {
	qsort(elements.data, elements.getcount(), sizeof(elements[0]), compare);
}

void answer_info::addv(int param, const char* format, const char* format_param) {
	if(elements.getcount() >= elements.getmaximum())
		return;
	if(get() >= end())
		return;
	if(get() > begin())
		addsz();
	auto pe = elements.add();
	pe->param = param;
	pe->text = get();
	stringcreator::addv(format, format_param);
}

void answer_info::add(int param, const char* format, ...) {
	addv(param, format, xva_start(format));
}

int	answer_info::choose(const char* format, const player_info* player) const {
	if(player!=player_info::gethuman()) {
		if(elements.getcount()==0)
			return 0;
		return elements.data[rand() % elements.getcount()].param;
	}
	if(!player)
		return 0;
	return choosev(false, 0, player->getid(), format);
}