#include "draw.h"

void draw::scrollv(const rect& scroll, int& origin, int count, int maximum, bool focused) {
	// count - elements per page
	// maximum - maximum elements
	if(!maximum || maximum == count)
		return;
	int p1;
	int ss = (scroll.height()*count) / maximum; // scroll size (in pixels)
	int ds = scroll.height() - ss;
	int dr = maximum - count;
	int p = (origin*ds) / dr + scroll.y1;
	auto a = area(scroll);
	auto need_correct = false;
	if(drag::active(scroll)) {
		a = AreaHilitedPressed;
		p1 = hot.mouse.y - drag::value;
		origin = ((p1 - scroll.y1)*dr) / ds;
		need_correct = true;
	} else if(a == AreaHilitedPressed && hot.key == MouseLeft) {
		if(hot.mouse.y < p)
			origin -= count;
		else if(hot.mouse.y > p + ss)
			origin += count;
		else {
			drag::begin(scroll);
			drag::value = hot.mouse.y - p;
		}
		need_correct = true;
	}
	// show scroll area
	switch(a) {
	case AreaHilited:
		rectf({scroll.x1, scroll.y1, scroll.x2 + 1, scroll.y2 + 1}, colors::button, 128);
		buttonv({scroll.x1, p, scroll.x2, p + ss}, false, false, false, true, 0);
		break;
	case AreaHilitedPressed:
		rectf({scroll.x1, scroll.y1, scroll.x2 + 1, scroll.y2 + 1}, colors::button, 128);
		buttonv({scroll.x1, p, scroll.x2, p + ss}, true, false, false, true, 0);
		break;
	default:
		if(focused)
			rectf({scroll.x2 - 2, p, scroll.x2 + 2, p + ss}, colors::blue, 128);
		break;
	}
	if(need_correct) {
		if(origin < 0)
			origin = 0;
		if(origin + count > maximum)
			origin = maximum - count;
	}
}

void draw::scrollh(const rect& scroll, int& origin, int per_page, int maximum, bool focused) {
	if(!maximum)
		return;
	int p1;
	int ss = ((scroll.x2 - scroll.x1)*per_page) / maximum;
	int ds = scroll.x2 - scroll.x1 - ss;
	int dr = maximum - per_page;
	if(!dr)
		return;
	int p = (origin*ds) / dr + scroll.x1;
	areas a = area(scroll);
	auto need_correct = false;
	if(drag::active(scroll)) {
		a = AreaHilitedPressed;
		p1 = hot.mouse.x - drag::value;
		origin = ((p1 - scroll.x1)*dr) / ds;
		need_correct = true;
	} else if(a == AreaHilitedPressed && hot.key == MouseLeft) {
		if(hot.mouse.x < p)
			origin -= per_page;
		else if(hot.mouse.x > p + ss)
			origin += per_page;
		else {
			drag::begin(scroll);
			drag::value = hot.mouse.x - p;
		}
		need_correct = true;
	} else if(a == AreaHilited || a == AreaHilitedPressed) {
		auto inc = per_page / 10;
		if(inc < 1)
			inc = 1;
		switch(hot.key) {
		case MouseWheelUp:
			origin -= inc;
			need_correct = true;
			break;
		case MouseWheelDown:
			origin += inc;
			need_correct = true;
			break;
		}
	}
	if(need_correct) {
		if(origin < 0)
			origin = 0;
		if(origin + per_page > maximum)
			origin = maximum - per_page;
	}
	switch(a) {
	case AreaHilited:
		rectf({scroll.x1, scroll.y1, scroll.x2 + 1, scroll.y2 + 1}, colors::button, 128);
		buttonh({p, scroll.y1, p + ss, scroll.y2}, false, false, false, true, 0);
		break;
	case AreaHilitedPressed:
		rectf({scroll.x1, scroll.y1, scroll.x2 + 1, scroll.y2 + 1}, colors::button, 128);
		buttonh({p, scroll.y1, p + ss, scroll.y2}, true, false, false, true, 0);
		break;
	default:
		if(focused)
			rectf({p, scroll.y2 - 2, p + ss, scroll.y2 + 2}, colors::blue, 128);
		break;
	}
}