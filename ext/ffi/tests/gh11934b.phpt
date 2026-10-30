--TEST--
Feature GH-11934 (Allow to pass CData into C variables)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
require_once __DIR__ . '/utils.inc';
$header = <<<HEADER
extern int gh11934b_ffi_var_test_cdata;
HEADER;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($header);
} else {
    try {
        $ffi = FFI::cdef($header, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($header, ffi_get_php_dll_name());
    }
}

$ffi->gh11934b_ffi_var_test_cdata->cdata = 2;
var_dump($ffi->gh11934b_ffi_var_test_cdata);
$source = $ffi->new('int');
$source->cdata = 31;
$ffi->gh11934b_ffi_var_test_cdata = $source;
var_dump($ffi->gh11934b_ffi_var_test_cdata);

?>
--EXPECT--
int(2)
int(31)
