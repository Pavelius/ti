#include "main.h"

answeri::answeri() : stringbuilder(buffer, zendof(buffer)) {}

int answeri::compare(const void* p1, const void* p2) {
	auto e1 = (answeri::element*)p1;
	auto e2 = (answeri::element*)p2;
	return strcmp(e1->text, e2->text);
}

void answeri::sort() {
	qsort(elements.data, elements.getcount(), sizeof(elements[0]), compare);
}

void answeri::addv(int param, const char* format, const char* format_param) {
	if(elements.getcount() >= elements.count_maximum)
		return;
	if(get() >= end())
		return;
	if(get() > begin())
		addsz();
	auto pe = elements.add();
	pe->param = param;
	pe->text = get();
	stringbuilder::addv(format, format_param);
}

void answeri::add(int param, const char* format, ...) {
	addv(param, format, xva_start(format));
}