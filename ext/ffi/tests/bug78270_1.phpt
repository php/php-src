--TEST--
FR #78270 (Usage of __vectorcall convention with FFI)
--SKIPIF--
<?php
require_once('skipif.inc');
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");

$dll = 'php8' . (PHP_ZTS ? 'ts' : '') . (PHP_DEBUG ? '_debug' : '') . '.dll';
try {
    FFI::cdef(<<<EOC
        __vectorcall int zend_atoi(const char *str, size_t str_len);
        EOC, $dll);
} catch (FFI\ParserException $ex) {
    die('skip __vectorcall not supported');
}
?>
--FILE--
<?php
$dll = 'php8' . (PHP_ZTS ? 'ts' : '') . (PHP_DEBUG ? '_debug' : '') . '.dll';
$ffi = FFI::cdef(<<<EOC
    __vectorcall int zend_atoi(const char *str, size_t str_len);
EOC, $dll);
var_dump($ffi->zend_atoi("17.4", 4));
?>
--EXPECT--
int(17)
