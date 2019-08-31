--TEST--
FFI 100: PHP symbols
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php
try {
	FFI::cdef("extern void *zend_printf;");
} catch (Throwable $e) {
	die('skip PHP symbols not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
// Check if target supports "fastcall" calling convention
try {
	FFI::cdef("extern size_t __attribute__((fastcall)) (*zend_printf)(const char *format);");
	$fastcall = "__attribute__((fastcall)) ";
} catch (Throwable $e) {
	$fastcall = "";
}
$zend = FFI::cdef("
	const char *get_zend_version(void);
	//char *get_zend_version(void);
	extern size_t (*zend_printf)(const char *format, ...);

	unsigned long $fastcall zend_hash_func(const char *str, size_t len);

	void $fastcall zend_str_tolower(char *str, size_t length);

");
var_dump(trim(explode("\n",$zend->get_zend_version())[0]));
//var_dump(trim(FFI::string($zend->get_zend_version())));
var_dump($zend->zend_printf);
var_dump(($zend->zend_printf)("Hello %s!\n", "World"));

var_dump($zend->zend_hash_func("file", strlen("file")));

$str = $zend->new("char[16]");
FFI::memcpy($str, "Hello World!", strlen("Hello World!"));
$zend->zend_str_tolower($str, strlen("Hello World!"));
var_dump(FFI::string($str));

?>
--EXPECTF--
string(%d) "Zend Engine %s"
object(FFI\CData:uint%d_t(*)())#%d (1) {
  [0]=>
  object(FFI\CData:uint%d_t())#%d (0) {
  }
}
Hello World!
int(13)
int(%i)
string(12) "hello world!"
