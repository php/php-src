--TEST--
Bug #77768 (Redeclaration of builtin types and repeated declarations)
--SKIPIF--
<?php
require_once('skipif.inc');
try {
	$libc = FFI::cdef("int printf(const char *format, ...);", "libc.so.6");
} catch (Throwable $_) {
	die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::cdef("
typedef __builtin_va_list __gnuc_va_list;
typedef unsigned int uint8_t;

typedef int64_t a;
typedef int64_t b;
typedef a c;
typedef b c;

struct point {int x,y;};

typedef struct point d;
typedef struct point d;

int printf(const char *format, ...);
int printf(const char *format, ...);
");

var_dump(FFI::sizeof($x->new("uint8_t")));
var_dump(FFI::sizeof(FFI::new("uint8_t")));
?>
--EXPECT--
int(4)
int(1)
