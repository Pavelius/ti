#include "crt.h"
#include "draw_control.h"

using namespace draw::controls;

bool column::operator==(const char* id) const {
	return strcmp(this->id, id) == 0;
}

int	column::gettotalwidth() const {
	auto result = 0;
	for(auto p = this; *p; p++)
		result += p->width;
	return result;
}

const char* table::getheader(char* result, const char* result_max, int column) const {
	return columns[column].name;
}

int table::rowheader(const rect& rc_original) const {
	draw::state push;
	fore = colors::special;
	char temp[260];
	auto height = getrowheight();
	auto rc = rc_original;
	rc.offset(1, 1);
	rc.offset(4, 4);
	for(auto i = 0; columns[i]; i++) {
		temp[0] = 0;
		auto p = getheader(temp, temp + sizeof(temp) / sizeof(temp[0]) - 1, i);
		if(p)
			draw::textc(rc.x1, rc.y1, columns[i].width, p, -1, columns[i].flags);
		rc.x1 += columns[i].width;
	}
	return height;
}

void table::row(const rect &rc_origin, int index) {
	auto rc = rc_origin;
	char temp[260];
	rowhilite(rc, index);
	rc.offset(4, 4);
	for(auto i = 0; columns[i]; i++) {
		rect rt = {rc.x1, rc.y1, rc.x1 + columns[i].width - 4, rc.y2};
		temp[0] = 0;
		const char* p;
		int number_value;
		switch(columns[i].getcontol()) {
		case Text:
			p = getname(temp, temp + sizeof(temp) / sizeof(temp[0]) - 1, index, i);
			if(p)
				draw::text(rt, p, columns[i].flags);
			break;
		case Number:
			number_value = getnumber(index, i);
			if((columns[i].flags & HideZero) == 0 || number_value != 0) {
				szprint(temp, temp + sizeof(temp) + 1, "%1i", number_value);
				draw::text(rt, temp, columns[i].flags);
			}
			break;
		}
		rc.x1 += columns[i].width;
	}
}

//void table::viewtotal(rect rc) const {
//	rc.offset(1, 1);
//	rc.offset(4, 4);
//	for(auto i = 0; columns[i].title; i++) {
//		char temp[260]; temp[0] = 0;
//		rect rt = {rc.x1, rc.y1, rc.x1 + columns[i].width - 4, rc.y2};
//		auto p = gettotal(temp, temp + sizeof(temp) - 1, i);
//		if(!p) {
//			auto result = gettotal(i);
//			if(result) {
//				szprints(temp, temp + sizeof(temp) - 1, "%1i", result);
//				p = temp;
//			}
//		}
//		if(p)
//			draw::text(rt, p, columns[i].flags);
//		rc.x1 += columns[i].width;
//	}
//}