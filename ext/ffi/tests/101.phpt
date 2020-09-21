--TEST--
FFI 101: PHP symbols (function address)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('utils.inc'); ?>
<?php
try {
    ffi_cdef("extern void *zend_printf;", ffi_get_php_dll_name());
} catch (Throwable $e) {
    die('skip PHP symbols not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
require_once('utils.inc');
$fastcall = ffi_get_fastcall_specifier();
$zend = ffi_cdef("
    const char *get_zend_version(void);
    //char *get_zend_version(void);
    extern size_t (*zend_printf)(const char *format, ...);

    unsigned long $fastcall zend_hash_func(const char *str, size_t len);

    void $fastcall zend_str_tolower(char *str, size_t length);

", ffi_get_php_dll_name());
$f = $zend->get_zend_version;
var_dump(trim(explode("\n",$f())[0]));
//var_dump(trim(FFI::string($zend->get_zend_version())));
var_dump($zend->zend_printf);
var_dump(($zend->zend_printf)("Hello %s!\n", "World"));

$f = $zend->zend_hash_func;
var_dump($f("file", strlen("file")));

$str = $zend->new("char[16]");
FFI::memcpy($str, "Hello World!", strlen("Hello World!"));
$f = $zend->zend_str_tolower;
$f($str, strlen("Hello World!"));
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
