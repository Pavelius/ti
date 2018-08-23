#pragma once

extern "C" void* malloc(unsigned size);
extern "C" void* realloc(void *ptr, unsigned size);
extern "C" void	free(void* pointer);

template <class T1, class T2>
class amap {
	struct element {
		T1 key;
		T2 value;
	};
	element* data;
	unsigned count;
	unsigned count_maximum;
	static unsigned getoptimal(unsigned need_count) {
		unsigned count_maximum = 64;
		while(count_maximum < 256 * 256 * 256) {
			if(need_count < count_maximum)
				break;
			count_maximum = count_maximum << 1;
		}
		return count_maximum;
	}
	void reserve() {
		if(data && count_maximum >= count)
			return;
		count_maximum = getoptimal(count+1);
		if(!data)
			data = (element*)malloc(count_maximum * sizeof(element));
		else
			data = (element*)realloc(data, count_maximum * sizeof(element));
	}
public:
	typedef T1 key_type;
	typedef T2 value_type;
	constexpr amap() : data(0), count(0) {}
	~amap() { if(data) free(data); }
	constexpr element* begin() { return data; }
	constexpr element* end() { return data + count; }
	T2* add(T1 k) {
		reserve();
		auto p = data + (count++);
		p->key = k;
		return &p->value;
	}
	T2* find(T1 k) {
		for(auto& e : *this) {
			if(e.key == k)
				return &e.value;
		}
		return 0;
	}
};