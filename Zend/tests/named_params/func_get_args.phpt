--TEST--
Behavior of func_get_args() and friends with named parameters
--FILE--
<?php

function test($a, $b = 'b', $c = 'c') {
    var_dump(func_num_args());
    var_dump(func_get_args());
    var_dump(func_get_arg(0));
    var_dump(func_get_arg(1));
    var_dump(func_get_arg(2));
}

test(c: 'C', a: 'A');

?>
--EXPECT--
int(3)
array(3) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "C"
}
string(1) "A"
string(1) "b"
string(1) "C"
