#include "crt.h"

#pragma once

namespace code {
struct metadata;
struct requisit {
	metadata*				parent;
	metadata*				type;
	const char*				id; // Identificator, must be filled
	unsigned				offset;
	unsigned				count;
	constexpr operator bool() const { return id != 0; }
	unsigned				getsize() const;
	unsigned				getsizeof() const { return getsize() * count; }
	constexpr void*			ptr(void* object) const { return (char*)object + offset; }
	void*					ptr(void* object, unsigned index) const { return (char*)object + offset + index * getsize(); }
	requisit*				setcount(int v) { if(this) count = v; return this; }
};
struct metadata {
	metadata*				type;
	const char*				id;
	unsigned				size;
	requisit*				add(const char* id, metadata* type);
	static void				initialize();
	bool					is(const char* id) const;
	bool					isarray() const { return id[0] == '[' && id[1] == ']' && id[2]==0; }
	bool					isnumber() const;
	bool					isreference() const { return id[0] == '*' && id[1] == 0; }
	bool					ispredefined() const;
	bool					istext() const;
	requisit*				find(const char* id) const;
	const requisit*			getid() const;
	const metadata*			gettype() const;
	void					update();
	void					write(const char* url) const;
	static void				write(const char* url, arem<metadata*>& types);
};
}