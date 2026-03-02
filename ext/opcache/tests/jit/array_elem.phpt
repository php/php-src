--TEST--
Refcount inference when adding array elements
--EXTENSIONS--
opcache
--FILE--
<?php

function test($a) {
    $ary = [$a];
    $ary2 = [0, $ary, $ary];
    return $ary2;
}
var_dump(test(1));

?>
--EXPECT--
array(3) {
  [0]=>
  int(0)
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
