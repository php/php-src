--TEST--
Scope function passed as callback to array_map
--FILE--
<?php
function test() {
    $factor = 3;
    $fn = fn(int $v): int { return $v * $factor; };
    var_dump(array_map($fn, [1, 2, 3]));
}
test();
?>
--EXPECT--
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(6)
  [2]=>
  int(9)
}
