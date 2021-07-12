--TEST--
FR #78270 (Usage of __vectorcall convention with FFI)
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");

require_once('utils.inc');
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
$x86 = (PHP_INT_SIZE === 4);
$arglists = array(
    'int, int, int, int, int, int, int' => true,
    'double, int, int, int, int, int, int' => !$x86,
    'int, double, int, int, int, int, int' => !$x86,
    'int, int, double, int, int, int, int' => !$x86,
    'int, int, int, double, int, int, int' => !$x86,
    'int, int, int, int, double, int, int' => false,
    'int, int, int, int, int, double, int' => false,
    'int, int, int, int, int, int, double' => true,
);
foreach ($arglists as $arglist => $allowed) {
    $signature = "__vectorcall void foobar($arglist);";
    try {
        $ffi = FFI::cdef($signature);
    } catch (FFI\ParserException $ex) {
        if ($allowed) {
            echo "($arglist): unexpected ParserException\n";
        }
    } catch (FFI\Exception $ex) {
        if (!$allowed) {
            echo "($arglist): unexpected Exception\n";
        }
    }
}
?>
--EXPECT--
