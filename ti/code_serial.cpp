#include "code.h"
#include "io.h"

using namespace code;

#include "io.h"

#pragma once

// Fast and simple driver for streaming binary data
struct archive {
	io::stream&			source;
	bool				writemode;
	constexpr archive(io::stream& source, bool writemode) : source(source), writemode(writemode) {}
	
	void setstring(const char** e) {
		unsigned len = 0;
		char temp[128 * 128];
		if(writemode) {
			if(*e)
				len = zlen(*e);
			source.write(&len, sizeof(len));
			if(len)
				source.write(*e, len);
		} else {
			source.read(&len, sizeof(len));
			e = 0;
			if(len) {
				source.read(temp, len);
				temp[len] = 0;
				*e = szdup(temp);
			}
		}
	}

	void setpointer(void** value) {
	}

	void set(void* value, unsigned size) {
		if(writemode)
			source.write(value, size);
		else
			source.read(value, size);
	}
	
	// Any pointer class
	template<class T> void set(T*& value) {
		setpointer((void**)&value);
	}
	// Strings case
	template<> void set<const char>(const char*& e) {
		setstring(&e);
	}
	// Array with fixed count
	template<typename T, unsigned N> void set(T(&value)[N]) {
		for(int i = 0; i < N; i++)
			set(value[i]);
	};
	// Fixed data collection
	template<typename T, unsigned N> void set(adat<T, N>& value) {
		set(value.count);
		for(auto& e : value)
			set(e);
	}
	// Fixed amem collection
	template<typename T> void set(arem<T>& value) {
		set(value.count);
		if(!writemode)
			value.reserve(value.count);
		for(auto& e : value)
			set(e);
	}
	// Custom aref collection
	template<typename T> void set(aref<T>& value) {
		set(value.count);
		set(value.data);
	}
	// All simple types and requisites
	template<class T> void set(T& value) {
		set(&value, sizeof(value));
	}
};

static void prepare(metadata* type, arem<metadata*>& source) {
	if(!type)
		return;
	if(source.indexof(type) != -1)
		return;
	source.add(type);
	prepare(type->type, source);
	for(auto& e : bsmeta<requisit>()) {
		if(!e)
			continue;
		if(e.parent != type)
			continue;
		prepare(e.type, source);
	}
}

void metadata::write(const char* url) const {
	io::file file(url, StreamWrite);
	if(!file)
		return;
	arem<metadata*> types;
	prepare(const_cast<metadata*>(this), types);
	archive a(file, true);
}