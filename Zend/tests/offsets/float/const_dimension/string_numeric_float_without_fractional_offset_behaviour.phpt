--TEST--
Float without fractional part offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_const_string_numeric_float_without_fractional_offsets.inc';

?>
--EXPECTF_EXTERNAL--
../expect.txt
