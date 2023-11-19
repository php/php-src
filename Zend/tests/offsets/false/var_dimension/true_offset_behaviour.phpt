--TEST--
true offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'dimension_var_true.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_variable_offsets.inc';

?>
--EXPECTF_EXTERNAL--
../expect_valid_offsets.txt
