--TEST--
Indexing -- Convert undefined variable to array
--FILE--
<?php
require 'indexing_testing.inc';
$value = array(1);

run_test('undefined', $value, true);
run_test_ref('undefined', $value, true);

run_test('undefined', $value, false);
run_test_ref('undefined', $value, false);

?>
--EXPECTF--
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
