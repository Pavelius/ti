#include "code.h"

using namespace code;

DECLBASE(metadata, 1024 * 8);

const unsigned pointer_size = sizeof(void*);
const unsigned array_size = pointer_size * 2;

bool isnum(char sym) {
	return sym >= '0' && sym <= '9';
}

requisit* metadata::add(const char* id, metadata* type) {
	auto p = find(id);
	if(p)
		return p;
	p = bsmeta<requisit>::add();
	p->id = id;
	p->parent = this;
	p->offset = size;
	p->type = type;
	size += p->getsize();
	return p;
}

requisit* metadata::add(const char* id, const char* type) {
	auto p = addtype(type);
	if(!p)
		return 0;
	return add(id, p);
}

requisit* metadata::find(const char* id) const {
	for(auto& e : bsmeta<requisit>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

metadata* metadata::findtype(const char* id) {
	for(auto& e : bsmeta<metadata>()) {
		if(!e)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

metadata* metadata::findtype(const char* id, const metadata* type) {
	for(auto& e : bsmeta<metadata>()) {
		if(!e)
			continue;
		if(e.type != type)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static metadata* add_type(const char* id, metadata* type, unsigned count) {
	auto p = metadata::findtype(id);
	if(p)
		return p;
	p = bsmeta<metadata>::add();
	p->id = szdup(id);
	p->count = count;
	if(type) {
		p->type = type;
		if(p->isarray() || p->isreference())
			p->size = pointer_size;
		else
			p->size = type->size;
	} else {
		p->type = 0;
		p->size = 0;
	}
	return p;
}

metadata* metadata::addtype(const char* id) {
	char temp[1024];
	auto ps = temp, pe = ps + sizeof(temp) - 1;
	auto p = id;
	while(*p) {
		if(*p == '*' || *p == '[')
			break;
		if(ps < pe)
			*ps++ = *p;
		p++;
	}
	*ps = 0;
	auto type = add_type(temp, 0, 1);
	while(*p) {
		if(*p == '*') {
			type = type->reference();
			p++;
		} else if(p[0] == '[' && p[1] == ']') {
			type = type->array();
			p += 2;
		} else if(p[0] == '[' && isnum(p[1])) {
			int i;
			p = stringbuilder::readint(p + 1, i);
			if(p[0] == ']')
				p++;
			type = type->array(i);
		} else
			return 0;
	}
	return type;
}

void metadata::primitive(const char* id, unsigned size) {
	auto p = metadata::findtype(id);
	if(!p)
		p = bsmeta<metadata>::add();
	if(!p)
		return;
	p->id = szdup(id);
	p->size = size;
}

void metadata::getname(stringbuilder& sb) const {
	if(isreference() || isarray())
		type->getname(sb);
	sb.add(id);
}

void metadata::initialize() {
	primitive("Void", 0);
	primitive("Text", pointer_size);
	primitive("Integer", 4);
	primitive("Short", 2);
	primitive("Byte", 1);
}

metadata* metadata::reference() const {
	auto p = findtype("*", this);
	if(p)
		return p;
	return add_type("*", const_cast<metadata*>(this), 1);
}

metadata* metadata::array() const {
	auto p = findtype("[]", this);
	if(p)
		return p;
	return add_type("[]", const_cast<metadata*>(this), 1);
}

metadata* metadata::array(int i) const {
	for(auto& e : bsmeta<metadata>()) {
		if(!e)
			continue;
		if(e.type != type)
			continue;
		if(e.count != i)
			continue;
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return add_type("[]", const_cast<metadata*>(this), i);
}

unsigned metadata::getcount() const {
	unsigned result = 0;
	for(auto& e : bsmeta<requisit>()) {
		if(!e)
			continue;
		if(e.parent == this)
			result++;
	}
	return result;
}