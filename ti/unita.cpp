#include "main.h"

void unita::sort(int (uniti::*proc)() const) {
	const auto i2 = maximum - 1;
	const auto i1 = 0;
	for(int i = i2; i > i1; i--) {
		for(int j = i1; j < i; j++) {
			auto t1 = (data[j]->*proc)();
			auto t2 = (data[j + 1]->*proc)();
			if(t1 > t2)
				iswap(data[j], data[j + 1]);
		}
	}
}

void unita::removecasualty(const playeri* player) {
	unita temp;
	for(auto p : *this) {
		if(!*p)
			continue;
		temp.add(p);
	}
	temp.sort(&uniti::getweight);
	if(temp.data[0])
		temp.data[0]->destroy();
}

void unita::rollup() {
	auto ps = data;
	auto pe = data + count;
	for(auto pd = data; pd < pe; pd++) {
		auto found = false;
		for(auto pb = data; pb < ps; pb++) {
			if(*pb == *ps) {
				found = true;
				break;
			}
		}
		if(!found)
			*ps++ = *pd;
	}
}

//void army::transform(target_s v) {
//	auto ps = data;
//	auto pe = data + count;
//	while(ps < pe) {
//		*ps = (*ps)->get(v);
//		ps++;
//	}
//}