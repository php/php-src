--TEST--
Indexing -- Convert boolean variable to array
--FILE--
<?php
$value = array(1);
require 'indexing_testing.inc';

run_test(true, $value, true);
run_test(false, $value, true);

run_test_ref(true, $value, true);
run_test_ref(false, $value, true);

run_test(true, $value, false);
run_test(false, $value, false);

run_test_ref(true, $value, false);
run_test_ref(false, $value, false);

?>
--EXPECTF--
Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
