#include "main.h"

void weapon_info::clear() {
	chance = 0;
	count = 1;
}

int weapon_info::roll() const {
	if(chance < 1)
		return 0;
	auto result = 0;
	for(auto i = 0; i < count; i++) {
		auto rr = reroll + 1;
		while(rr-- > 0) {
			if(((1 + (rand() % 10)) + bonus) >= chance) {
				result++;
				break;
			}
		}
	}
	return result;
}