--TEST--
true offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_const_true_offsets.inc';

?>
--EXPECTF_EXTERNAL--
../expect_cast_occurred_offsets.txt
