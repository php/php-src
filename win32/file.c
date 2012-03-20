#include "ruby/ruby.h"
#include <winbase.h>

int
rb_file_load_ok(const char *path)
{
    int ret = 1;
    DWORD attr = GetFileAttributes(path);
    if (attr == INVALID_FILE_ATTRIBUTES ||
	attr & FILE_ATTRIBUTE_DIRECTORY) {
	ret = 0;
    }
    else {
	HANDLE h = CreateFile(path, GENERIC_READ,
			      FILE_SHARE_READ | FILE_SHARE_WRITE,
			      NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h != INVALID_HANDLE_VALUE) {
	    CloseHandle(h);
	}
	else {
	    ret = 0;
	}
    }
    return ret;
}
