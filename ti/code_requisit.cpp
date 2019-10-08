#include "code.h"

using namespace code;

DECLBASE(requisit, 1024 * 64);

unsigned requisit::getsize() const {
	return type->size;
}