#include "crt.h"
#include "stringbuilder.h"

#pragma once

namespace code {
struct metadata;
struct requisit {
	const char*				id; // Identificator, must be filled
	metadata*				type;
	metadata*				parent;
	unsigned				offset;
	unsigned				count;
	constexpr operator bool() const { return id != 0; }
	unsigned				getsize() const;
	unsigned				getsizeof() const { return getsize() * count; }
	constexpr void*			ptr(void* object) const { return (char*)object + offset; }
	void*					ptr(void* object, unsigned index) const { return (char*)object + offset + index * getsize(); }
	requisit*				setcount(int v) { if(this) count = v; return this; }
	void					write(const char* url) const;
};
struct metadata {
	metadata*				type;
	const char*				id;
	unsigned				size;
	constexpr operator bool() const { return id != 0; }
	requisit*				add(const char* id, metadata* type, unsigned count = 1);
	requisit*				add(const char* id, const char* type, unsigned count = 1);
	static metadata*		addtype(const char* id);
	metadata*				array() const;
	unsigned				getcount() const;
	void					getname(stringbuilder& sb) const;
	static void				initialize();
	bool					isarray() const { return id[0] == '[' && id[1] == ']'; }
	bool					isreference() const { return id[0] == '*'; }
	requisit*				find(const char* id) const;
	static metadata*		findtype(const char* id);
	static metadata*		findtype(const char* id, const metadata* type);
	static bool				read(const char* url);
	metadata*				reference() const;
	void					write(const char* url) const;
};
}