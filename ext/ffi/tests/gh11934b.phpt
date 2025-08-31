--TEST--
Feature GH-11934 (Allow to pass CData into C variables)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
$header = <<<HEADER
extern int gh11934b_ffi_var_test_cdata;
HEADER;

$ffi = FFI::cdef($header);
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
