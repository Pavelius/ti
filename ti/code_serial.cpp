#include "code.h"
#include "io.h"

using namespace code;

#include "io.h"

#pragma once

// Fast and simple driver for streaming binary data
struct archive {
	typedef arem<metadata*> metadataa;
	io::stream&			source;
	bool				writemode;
	metadataa			types;
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
			*e = 0;
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
	// Custom aref collection
	void set(metadataa& value) {
		set(value.count);
		if(!writemode)
			value.reserve(value.count);
		unsigned len;
		for(auto& e : value) {
			char temp[2048]; temp[0] = 0;
			if(!writemode) {
				source.read(&len, sizeof(len));
				e = 0;
				if(len) {
					source.read(temp, len);
					temp[len] = 0;
				}
				e = metadata::addtype(temp);
			} else {
				stringbuilder sb(temp);
				e->getname(sb);
				len = zlen(sb.begin());
				source.write(&len, sizeof(len));
				source.write(temp, len);
			}
		}
	}
	template<> void set<metadata>(metadata*& value) {
		unsigned data;
		if(writemode) {
			data = types.indexof(value);
			source.write(&data, sizeof(data));
		} else {
			source.read(&data, sizeof(data));
			if(data == 0xFFFFFFFF)
				value = 0;
			else
				value = types[data];
		}
	}
	void setex(requisit& e) {
		set(e.parent);
		set(e.id);
		set(e.count);
		set(e.type);
	}
	void setreq() {
		requisit e1;
		setex(e1);
		e1.parent->add(e1.id, e1.type, e1.count);
	}
	template<> void set<requisit>(requisit& e) {
		if(writemode)
			setex(e);
		else
			setreq();
	}
	void prepare(metadata* type) {
		if(!type)
			return;
		if(types.indexof(type) != -1)
			return;
		types.add(type);
		prepare(type->type);
		for(auto& e : bsmeta<requisit>()) {
			if(!e)
				continue;
			if(e.parent != type)
				continue;
			prepare(e.type);
		}
	}
	void setreq(metadata* p) {
		if(writemode) {
			auto count = p->getcount();
			source.write(&count, sizeof(count));
			for(auto& e : bsmeta<requisit>()) {
				if(!e)
					continue;
				if(e.parent != p)
					continue;
				set(e);
			}
		} else {
			unsigned count;
			source.read(count);
			for(unsigned i = 0; i < count; i++)
				setreq();
		}
	}
};

void metadata::write(const char* url) const {
	io::file file(url, StreamWrite);
	if(!file)
		return;
	archive a(file, true);
	a.prepare(const_cast<metadata*>(this));
	a.set(a.types);
	a.setreq(const_cast<metadata*>(this));
}

bool metadata::read(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return false;
	archive a(file, false);
	a.set(a.types);
	a.setreq(0);
	return true;
}