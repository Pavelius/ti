#include "main.h"

unsigned char abstract_deck::draw() {
	if(!count)
		shuffle();
	if(!count)
		return 0;
	auto e = data[0];
	remove(0);
	return e;
}

void abstract_deck::clear() {
	adat<unsigned char>::clear();
	discarded.clear();
}

void abstract_deck::discard(unsigned char e) {
	discarded.add(e);
}

void abstract_deck::shuffle() {
	if(discarded.count > 0) {
		for(auto e : discarded)
			add(e);
		discarded.clear();
	}
	if(count > 0)
		zshuffle(data, count);
}