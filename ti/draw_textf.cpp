#include "crt.h"
#include "draw.h"
#include "stringbuilder.h"

using namespace draw;

static const char* skipcr(const char* p) {
	if(*p == 10) {
		p++;
		if(*p == 13)
			p++;
	} else if(*p == 13) {
		p++;
		if(*p == 10)
			p++;
	}
	return p;
}

static const char* skipsp(const char* p) {
	while(*p == ' ' || *p == 9) p++; return p;
}

static bool isnum(const char* p) {
	return *p >= '0' && *p <= '9';
}

const char* sz2num(const char* p, int& result) {
	result = 0;
	auto sign = false;
	if(*p == ' ') {
		p++;
		sign = true;
	}
	while(isnum(p))
		result = result * 10 + (*p++) - '0';
	if(sign)
		result = -result;
	return p;
}

static const char* psstr(const char* p, char* ps, const char* pe, char end_symbol) {
	ps[0] = 0;
	if(!p)
		return 0;
	while(true) {
		if(*p == 0) {
			*ps = 0;
			return p;
		} else if(*p == end_symbol) {
			*ps = 0;
			return p + 1;
		} else if(*p == '\\') {
			p++;
			switch(*p) {
			case 'n':
				if(ps < pe)
					*ps++ = '\n';
				p++;
				break;
			case 'r':
				if(ps < pe)
					*ps++ = '\r';
				p++;
				break;
			case 't':
				if(ps < pe)
					*ps++ = '\t';
				p++;
				break;
			default:
				if(ps < pe)
					*ps++ = *p;
				p++;
				break;
			}
		} else {
			if(ps < pe)
				*ps++ = *p;
			p++;
		}
	}
}

textplugin*	draw::textplugin::first;
static void(*draw_icon)(int& x, int& y, int x0, int x2, int* max_width, int& w, const char* name);

void draw::set(void(*proc)(int& x, int& y, int x0, int x2, int* max_width, int& w, const char* id)) {
	draw_icon = proc;
}

textplugin::textplugin(const char* name, proc e) : name(name), render(e) {
	if(!first)
		first = this;
	else {
		auto p = first;
		while(p->next)
			p = p->next;
		p->next = this;
	}
}

textplugin* textplugin::textplugin::find(const char* name) {
	for(auto p = first; p; p = p->next) {
		if(strcmp(p->name, name) == 0)
			return p;
	}
	return 0;
}

static bool match(const char** string, const char* name) {
	int n = zlen(name);
	if(memcmp(*string, name, n) != 0)
		return false;
	(*string) += n;
	return true;
}

static int render_control(const char** result, int x, int y, int width) {
	struct element {
		const char*	id;
		const char*	label;
		const char*	tips;
		int			value;
	};
	element e = {0};
	char type[64];
	char name[64];
	char buffer[4096];
	auto p = *result;
	auto pb = buffer;
	auto pe = buffer + sizeof(buffer) - 1;
	p = stringbuilder::readid(p, type, type + sizeof(type) - 1);
	p = skipsp(p);
	while(*p && *p != ')') {
		p = stringbuilder::readid(p, name, name + sizeof(name) - 1);
		p = skipsp(p);
		auto value_number = 1;
		const char* value_text = 0;
		if(*p == '=') {
			p = skipsp(p + 1);
			if(isnum(p) || *p == '-')
				p = sz2num(p, value_number);
			else if(*p == '\"' || *p == '\'') {
				value_text = pb;
				p = psstr(p + 1, pb, pe, p[0]);
				pb = zend(pb);
				if(pb < pe)
					pb = pb + 1;
			} else {
				// Error
				while(*p != ')' && *p)
					p++;
			}
			p = skipsp(p);
		}
		if(strcmp("id", name) == 0)
			e.id = value_text;
		else if(strcmp("tips", name) == 0)
			e.tips = value_text;
		else if(strcmp("label", name) == 0)
			e.label = value_text;
		else if(strcmp("value", name) == 0)
			e.value = value_number;
	}
	if(*p == ')')
		p++;
	p = skipcr(p);
	*result = p;
	auto pm = draw::textplugin::find(type);
	if(pm)
		return pm->render(x, y, width, e.id, e.value, e.label, e.tips);
	return 0;
}

static const char* glink(const char* p, char* result, unsigned result_maximum) {
	result[0] = 0;
	if(*p == '\"') {
		auto sym = *p++;
		p = psstr(p, result, result + result_maximum - 1, sym);
	} else if(*p == '(') {
		auto ps = result;
		auto pe = ps + result_maximum;
		while(*p && *p != ')') {
			if(ps < pe)
				*ps++ = *p;
			p++;
		}
		*ps++ = 0;
		if(*p == ')')
			p++;
	}
	return p;
}

static const char* textspc(const char* p, int x0, int& x, int tab_width) {
	int tb;
	while(true) {
		switch(p[0]) {
		case ' ':
			p++;
			x += draw::textw(' ');
			continue;
		case '\t':
			p++;
			if(!tab_width)
				tb = draw::textw('A') * 3;
			else
				tb = tab_width;
			x = x0 + ((x - x0 + tb) / tb)*tb;
			continue;
		}
		break;
	}
	return p;
}

static const char* word(const char* text) {
	while(((unsigned char)*text) > 0x20 && *text != '*' && *text != '[' && *text != ']')
		text++;
	return text;
}

static int textfln(int x0, int y0, int width, const char** string, color c1, int* max_width, int tab_width, unsigned text_flags) {
	char temp[4096];
	int y = y0;
	int x = x0;
	int x2 = x0 + width;
	const char* p = *string;
	unsigned flags = text_flags;
	draw::fore = c1;
	if(max_width)
		*max_width = 0;
	temp[0] = 0;
	while(true) {
		if(p[0] == '*' && p[1] == '*') {
			p += 2;
			if(flags&TextBold)
				flags &= ~TextBold;
			else
				flags |= TextBold;
			continue;
		} else if(p[0] == '*') {
			p++;
			if(flags&TextItalic)
				flags &= ~TextItalic;
			else {
				if((flags&TextItalic) == 0)
					x += draw::texth() / 3;
				flags |= TextItalic;
			}
			continue;
		} else if(p[0] == '[' && p[1] == '[')
			p++;
		else if(p[0] == ']' && p[1] == ']')
			p++;
		else if(p[0] == '[') {
			p++;
			switch(*p) {
			case '~':
				p++;
				draw::fore = colors::text.mix(colors::window, 64);
				break;
			case '+':
				p++;
				draw::fore = colors::green;
				break;
			case '-':
				p++;
				draw::fore = colors::red;
				break;
			case '!':
				p++;
				draw::fore = colors::yellow;
				break;
			case '#':
				p++;
				flags |= TextUscope;
				draw::fore = colors::special;
				break;
			default:
				draw::fore = colors::special;
				break;
			}
			p = glink(p, temp, sizeof(temp) / sizeof(temp[0]) - 1);
		} else if(p[0] == ']') {
			p++;
			draw::fore = c1;
			temp[0] = 0;
			flags &= ~TextUscope;
		}
		// ���������� ������� � ���������
		p = textspc(p, x0, x, tab_width);
		int w;
		if(p[0] == ':' && p[1] >= 'a' && p[1] <= 'z') {
			p++;
			char temp[128];
			p = stringbuilder::readid(p, temp, temp + sizeof(temp) - 1);
			if(*p == ':')
				p++;
			w = 0;
			if(draw_icon)
				draw_icon(x, y, x0, x2, max_width, w, temp);
		} else {
			const char* p2 = word(p);
			w = draw::textw(p, p2 - p);
			if(x + w > x2) {
				if(max_width)
					*max_width = imax(*max_width, x - x0);
				x = x0;
				y += draw::texth();
			}
			draw::text(x, y, p, p2 - p, flags);
			p = p2;
		}
		int x4 = x;
		x += w;
		p = textspc(p, x0, x, tab_width);
		if(temp[0] || (flags&TextUscope) != 0) {
			int x3 = imin(x2, x);
			int y2 = y + draw::texth();
			if(flags&TextUscope)
				draw::line(x4, y2, x3, y2, draw::fore);
			rect rc = {x4, y, x3, y2};
			if(draw::areb(rc)) {
				if(flags&TextUscope) {
					hot.cursor = CursorHand;
					if(temp[0] && hot.key == MouseLeft && !hot.pressed) {
						zcpy(draw::link, temp, sizeof(draw::link) - 1);
						//draw::execute(HtmlLink);
					}
				} else
					zcpy(draw::link, temp, sizeof(draw::link) - 1);
			}
		}
		// ������� ������� ������ � ��������� ������
		if(p[0] == 0 || p[0] == 10 || p[0] == 13) {
			y += draw::texth();
			p = skipcr(p);
			break;
		}
	}
	if(max_width)
		*max_width = imax(*max_width, x - x0);
	*string = p;
	return y - y0;
}

int draw::textf(int x, int y, int width, const char* string, int* max_width,
	int min_height, int* cashe_height, const char** cashe_string, int tab_width, unsigned text_flags) {
	state push;
	color color_text = fore;
	const char* p = string;
	int y0 = y;
	if(cashe_height) {
		*cashe_string = p;
		*cashe_height = 0;
	}
	if(max_width)
		*max_width = 0;
	while(p[0]) {
		int mw2 = 0;
		if(cashe_height && (y - y0) <= min_height) {
			*cashe_string = p;
			*cashe_height = y - y0;
		}
		if(match(&p, "###")) // Header 3
		{
			p = skipsp(p);
			font = metrics::h3;
			y += textfln(x, y, width, &p, colors::h3, &mw2, tab_width, text_flags);
		} else if(match(&p, "##")) // Header 2
		{
			p = skipsp(p);
			font = metrics::h2;
			y += textfln(x, y, width, &p, colors::h2, &mw2, tab_width, text_flags);
		} else if(match(&p, "#")) // Header 1
		{
			p = skipsp(p);
			font = metrics::h1;
			y += textfln(x, y, width, &p, colors::h1, &mw2, tab_width, text_flags);
		} else if(match(&p, "...")) // ��� ��������������
		{
			p = skipcr(p);
			font = metrics::font;
			color c1 = colors::window.mix(colors::edit, 256 - 32);
			y += texth() / 2;
			while(p[0]) {
				int c = textbc(p, width);
				if(!c)
					break;
				rectf({x, y, x + width, y + texth()}, c1);
				text(x, y, p, c);
				y += texth();
				p += c;
				if(match(&p, "...")) {
					p = skipcr(p);
					y += texth() / 2;
					break;
				}
			}
		} else if(match(&p, "* ")) {
			// ������
			int dx = texth() / 2;
			int rd = texth() / 6;
			circlef(x + dx + 2, y + dx, rd, color_text);
			circle(x + dx + 2, y + dx, rd, color_text);
			y += textfln(x + texth(), y, width - texth(), &p, color_text, &mw2, tab_width, text_flags);
		} else if(p[0] == '$' && p[1] == '(') {
			p = skipsp(p + 2);
			y += render_control(&p, x, y, width);
		} else
			y += textfln(x, y, width, &p, color_text, &mw2, tab_width, text_flags);
		// ��������� ����������� ��������� �����
		font = metrics::font;
		fore = color_text;
		if(max_width) {
			if(*max_width < mw2)
				*max_width = mw2;
		}
	}
	return y - y0;
}

int draw::textf(rect& rc, const char* string, int tab_width) {
	state push;
	clipping.clear();
	rc.y2 = rc.y1 + draw::textf(0, 0, rc.width(), string, &rc.x2, 0, 0, 0, tab_width);
	rc.x2 += rc.x1;
	return rc.height();
}

int draw::textfw(const char* string, int tab_width) {
	state push;
	clipping.clear();
	int width = 0;
	draw::textf(0, 0, 1000, string, &width, 0, 0, 0, tab_width);
	return width;
}