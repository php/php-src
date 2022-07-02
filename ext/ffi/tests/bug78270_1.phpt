--TEST--
FR #78270 (Usage of __vectorcall convention with FFI)
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");

require_once('utils.inc');
ob_start();
phpinfo(INFO_GENERAL);
$info = ob_get_clean();
if (preg_match('/Compiler => .*clang.*/', $info)) die("skip not for clang");

try {
    FFI::cdef(<<<EOC
        __vectorcall int bug78270(const char *str, size_t str_len);
        EOC, "php_zend_test.dll");
} catch (FFI\ParserException $ex) {
    die('skip __vectorcall not supported');
}
?>
--FILE--
<?php
require_once('utils.inc');
$ffi = FFI::cdef(<<<EOC
    __vectorcall int bug78270(const char *str, size_t str_len);
EOC, "php_zend_test.dll");
var_dump($ffi->bug78270("17.4", 4));
?>
--EXPECT--
int(17)
