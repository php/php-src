--TEST--
zend_test_call_with_consumed_args(): consume a non-reference arg
--EXTENSIONS--
zend_test
--FILE--
<?php

$result = zend_test_call_with_consumed_args(
    static fn($a, $b) => [$a, $b],
    [[1, 2, 3], "x"],
    1,
);

var_dump($result["consumed_args"]);
var_dump($result["retval"]);

?>
--EXPECT--
int(1)
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  string(1) "x"
}
