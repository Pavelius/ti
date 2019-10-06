#include "base.h"

unsigned basei::get(const void* object) {
	for(auto& e : bsmeta<basei>()) {
		if(!e.elements.size)
			continue;
		if(object >= e.elements.data && object < (char*)e.elements.data + e.elements.size)
			return ((&e - bsmeta<basei>::elements)<<24) + ((char*)object - (char*)e.elements.data) / e.elements.size;
	}
	return 0;
}