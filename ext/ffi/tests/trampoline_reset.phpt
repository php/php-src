--TEST--
Memory corruption when mixing __callStatic() and FFI
--EXTENSIONS--
ffi
--SKIPIF--
<?php
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
class Test
{
	public static function __callStatic($name, $args)
	{
		echo "$name called\n";
	}
}

$header = '
typedef struct _IO_FILE FILE;
extern FILE *stdout;
int fprintf(FILE *, const char *, ...);
int fflush(FILE *);
';
$ffi = FFI::cdef($header, 'libc.so.6');

Test::foo();
Test::bar();
$ffi->fprintf($ffi->stdout, "FFI\n");
$ffi->fflush($ffi->stdout);
Test::baz();
?>
--EXPECT--
foo called
bar called
FFI
baz called
