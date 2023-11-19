--TEST--
Numeric float with fractional part string offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_const_string_numeric_float_with_fractional_offsets.inc';

?>
--EXPECTF_EXTERNAL--
../expect_valid_offsets.txt
