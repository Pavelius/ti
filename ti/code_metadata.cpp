#include "code.h"

using namespace code;

DECLBASE(metadata, 1024 * 8);

const unsigned pointer_size = sizeof(void*);
const unsigned array_size = pointer_size * 2;

requisit* metadata::add(const char* id, metadata* type, unsigned count) {
	auto p = find(id);
	if(p)
		return p;
	p = bsmeta<requisit>::add();
	p->id = id;
	p->parent = this;
	p->offset = size;
	p->count = count;
	p->type = type;
	size += p->getsize();
	return p;
}

requisit* metadata::add(const char* id, const char* type, unsigned count) {
	auto p = addtype(type);
	if(!p)
		return 0;
	return add(id, p, count);
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

static metadata* add_type(const char* id, metadata* type) {
	auto p = metadata::findtype(id);
	if(p)
		return p;
	p = bsmeta<metadata>::add();
	p->id = szdup(id);
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
	auto type = add_type(temp, 0);
	while(*p) {
		if(*p == '*') {
			type = type->reference();
			p++;
		} else if(p[0] == '[' && p[1] == ']') {
			type = type->array();
			p += 2;
		} else
			return 0;
	}
	return type;
}

static void create(const char* id, unsigned size) {
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
	create("Void", 0);
	create("Text", pointer_size);
	create("Integer", 4);
	create("Short", 2);
	create("Byte", 1);
}

metadata* metadata::reference() const {
	auto p = findtype("*", this);
	if(p)
		return p;
	return add_type("*", const_cast<metadata*>(this));
}

metadata* metadata::array() const {
	auto p = findtype("[]", this);
	if(p)
		return p;
	return add_type("[]", const_cast<metadata*>(this));
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