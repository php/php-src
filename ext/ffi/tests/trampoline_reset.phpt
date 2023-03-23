--TEST--
Test
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

Test::works1();
Test::works2();

$ffi = FFI::cdef('int printf(const char *format, ...);', 'libc.so.6');
$ffi->printf("Hello %s!\n", "world");

Test::breaks();
?>
--EXPECT--
works1 called
works2 called
breaks called
Hello world!
