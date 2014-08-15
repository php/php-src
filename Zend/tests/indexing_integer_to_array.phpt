--TEST--
Indexing -- Convert integer variable to array
--FILE--
<?php
require 'indexing_testing.inc';
$value = array(1);

run_test(1, $value, true);
run_test(0, $value, true);

run_test_ref(1, $value, true);
run_test_ref(0, $value, true);

run_test(1, $value, false);
run_test(0, $value, false);

run_test_ref(1, $value, false);
run_test_ref(0, $value, false);

?>
--EXPECTF--
Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)
