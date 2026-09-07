--TEST--
zend_test_call_with_consumed_args(): ref-required params with reference args are not consumed
--EXTENSIONS--
zend_test
--FILE--
<?php

$value = [10];

$result = zend_test_call_with_consumed_args(
    static function (&$a) {
        $a[] = 20;
        return $a;
    },
    [&$value],
    1,
);

var_dump($result["consumed_args"]);
var_dump($result["retval"]);
var_dump($value);

?>
--EXPECT--
int(0)
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(20)
}
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(20)
}
