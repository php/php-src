--TEST--
SPL: ArrayObject indirect offsetGet overwriting EG(uninitialized_zvar_ptr)
--FILE--
<?php
$test = new ArrayObject();
$test['d1']['d2'] = 'hello';
$test['d1']['d3'] = 'world';
var_dump($test, $test3['mmmmm']);
?>
--EXPECTF--
Warning: Undefined variable $test3 in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    ["d1"]=>
    array(2) {
      ["d2"]=>
      string(5) "hello"
      ["d3"]=>
      string(5) "world"
    }
  }
}
NULL
