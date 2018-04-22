#include "crt.h"
#include "io.h"
#include "win.h"

void printc(const char* format, ...) {
	char temp[4096 * 4];
	szprintvs(temp, temp + sizeof(temp)/sizeof(temp[0]), format, xva_start(format));
	printcnf(temp);
}

void printcnf(const char* text) {
	unsigned result;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, zlen(text), &result, 0);
}

io::file::file() : handle(0) {
}

io::file::file(const char* url, unsigned flags) : file() {
	create(url, flags);
}

io::file::~file() {
	close();
}

io::file::find::find(const char* url) {
	char temp[261];
	zcpy(temp, url);
	zcat(temp, "/*.*");
	handle = FindFirstFileA(temp, (WIN32_FIND_DATA*)&reserved);
	if(handle == ((void*)-1))
		handle = 0;
}

void io::file::find::next() {
	if(!handle)
		return;
	if(!FindNextFileA(handle, (WIN32_FIND_DATA*)&reserved)) {
		FindClose(handle);
		handle = 0;
	}
}

io::file::find::~find() {
	if(handle)
		FindClose(handle);
	handle = 0;
}

const char* io::file::find::name() {
	return ((WIN32_FIND_DATA*)&reserved)->cFileName;
}

bool io::file::create(const char* url, unsigned flags) {
	if(handle)
		return true;
	handle = (int)CreateFileA(url,
		(flags&StreamWrite) ? GENERIC_WRITE : GENERIC_READ,
		0,
		0,
		(flags&StreamWrite) ? CREATE_ALWAYS : OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if(handle == -1)
		handle = 0;
	else {
		if((flags&(StreamText | StreamWrite)) == (StreamText | StreamWrite)) {
			static unsigned char header_utf8[] = {0xEF, 0xBB, 0xBF};
			write(header_utf8, sizeof(header_utf8));
		}
	}
	return (*this);
}

void io::file::close() {
	CloseHandle((void*)handle);
	handle = 0;
}

char* io::file::getdir(char* url, int size) {
	if(!GetCurrentDirectoryA(size, url))
		return 0;
	return url;
}

char* io::file::getmodule(char* url, int size) {
	if(!GetModuleFileNameA(0, url, size))
		return 0;
	return url;
}

bool io::file::setdir(const char* url) {
	return SetCurrentDirectoryA(url) != 0;
}

bool io::file::remove(const char* url) {
	return DeleteFileA(url) != 0;
}

int io::file::read(void* p, int size) {
	unsigned opsize;
	if(!ReadFile((void*)handle, p, size, &opsize, 0))
		return 0;
	return opsize;
}

int io::file::write(const void* p, int size) {
	unsigned opsize;
	if(!WriteFile((void*)handle, p, size, &opsize, 0))
		return 0;
	return opsize;
}

int io::file::seek(int pos, int rel) {
	int mm = FILE_BEGIN;
	switch(rel) {
	case SeekCur: mm = FILE_CURRENT; break;
	case SeekEnd: mm = FILE_END; break;
	}
	return SetFilePointer(handle, pos, 0, mm);
}

bool io::file::exist(const char* url) {
	return GetFileAttributesA(url) != 0xFFFFFFFF;
}

bool io::file::makedir(const char* url) {
	return CreateDirectoryA(url, 0) != 0;
}