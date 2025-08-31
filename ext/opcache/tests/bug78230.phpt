--TEST--
Bug #78230: Incorrect type check optimization
--EXTENSIONS--
opcache
--FILE--
<?php

function test($x) {
    $y = (array) $x;
    var_dump(is_array($y));
}

$ary = [1, 2];
$ary[] = 3;
test($ary);
$ary[] = 4;
var_dump($ary);

?>
--EXPECT--
bool(true)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
