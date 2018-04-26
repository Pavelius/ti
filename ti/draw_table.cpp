#include "crt.h"
#include "draw.h"

using namespace draw::controls;

void table::row(rect rc, int index) const {
	char temp[260];
	if(index == current)
		hilight(rc);
	else if(index == current_hilite)
		rectf({rc.x1, rc.y1, rc.x2, rc.y2 - 1}, colors::edit.mix(colors::window, 96));
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
				szprints(temp, temp + sizeof(temp) + 1, "%1i", number_value);
				draw::text(rt, temp, columns[i].flags);
			}
			break;
		default:
			custom(temp, temp + sizeof(temp) - 1, rc, index, i);
			break;
		}
		rc.x1 += columns[i].width;
	}
}

void table::viewtotal(rect rc) const {
	rc.offset(1, 1);
	rc.offset(4, 4);
	for(auto i = 0; columns[i].title; i++) {
		char temp[260]; temp[0] = 0;
		rect rt = {rc.x1, rc.y1, rc.x1 + columns[i].width - 4, rc.y2};
		auto p = gettotal(temp, temp + sizeof(temp) - 1, i);
		if(!p) {
			auto result = gettotal(i);
			if(result) {
				szprints(temp, temp + sizeof(temp) - 1, "%1i", result);
				p = temp;
			}
		}
		if(p)
			draw::text(rt, p, columns[i].flags);
		rc.x1 += columns[i].width;
	}
}

void table::view(rect rc) {
	if(show_totals) {
		list::view({rc.x1, rc.y1, rc.x2, rc.y2 - getrowheight()});
		viewtotal({rc.x1, rc.y2 - getrowheight(), rc.x2, rc.y2});
	} else
		list::view(rc);
}