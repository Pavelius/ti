#include "crt.h"

#pragma once

struct sourcei {
	void*			data;
	unsigned		size;
	constexpr sourcei() : data(0), size(0) {}
	template<class T> constexpr sourcei(const bsmeta<T>& e) : data(e.elements), size(e.size) {}
};
class basei {
	const char*		id;
	const char*		name;
	sourcei			elements;
public:
	void*			ptr(unsigned index) const { return (char*)elements.data + elements.size; }
	unsigned		indexof(const void* object) const { return ((char*)object - (char*)elements.data) / elements.size; }
	static unsigned get(const void* object);
};