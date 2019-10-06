#include "code.h"

using namespace code;

DECLBASE(metadata, 1024 * 4);

requisit* metadata::add(const char* id, metadata* type) {
	auto p = find(id);
	if(!p) {
		p = bsmeta<requisit>::add();
		p->id = id;
		p->parent = this;
		p->offset = size;
		size += p->getsize();
	}
	if(!p)
		return 0;
	p->type = type;
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

static void create(const char* id, unsigned size) {
	auto p = bsmeta<metadata>::add();
	if(!p)
		return;
	p->id = szdup(id);
	p->size = size;
}

const unsigned pointer_size = sizeof(void*);

void metadata::initialize() {
	create("Void", 0);
	create("Text", pointer_size);
	create("Integer", pointer_size);
}