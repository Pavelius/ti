#include "code.h"

using namespace code;

DECLBASE(metadata, 1024 * 4);

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

metadata* metadata::addtype(const char* id, metadata* type) {
	auto p = metadata::findtype(id);
	if(p)
		return p;
	p->id = szdup(id);
	if(type) {
		p->type = type;
		p->size = type->size;
	} else {
		p->type = 0;
		p->size = 0;
	}
	return p;
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

const unsigned pointer_size = sizeof(void*);

void metadata::initialize() {
	create("Void", 0);
	create("Text", pointer_size);
	create("Integer", 4);
	create("Short", 2);
	create("Byte", 1);
}