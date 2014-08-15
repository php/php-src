--TEST--
Indexing -- Convert float variable to array
--FILE--
<?php
require 'indexing_testing.inc';
$value = array(1);

run_test(0.1, $value, true, false);
run_test(0.1, $value, true, true);

run_test(0.1, $value, false, false);
run_test(0.1, $value, false, true);

?>
--EXPECTF--
Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)
