--TEST--
GH-25007 (PFA in constexpr: named args before a named placeholder)
--FILE--
<?php

function f($a, $b = 2, $c = 3) {
    var_dump(func_get_args());
}

const C1 = f(a: 1, b: ?);
const C2 = f(a: 1, c: ?, b: ?);

(C1)(4);
(C2)(5, 6);

?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(6)
  [2]=>
  int(5)
}
