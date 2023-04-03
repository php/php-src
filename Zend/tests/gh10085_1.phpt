--TEST--
GH-10085: Assertion in add_function_array()
--FILE--
<?php
$i = [[], 0];
$ref = &$i;
$i[0] += $ref;
var_dump($i);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    int(0)
  }
  [1]=>
  int(0)
}
