--TEST--
func_get_args with variable number of args
--FILE--
<?php

function foo($a)
{
    var_dump(func_get_args());
}
foo(1, 2, 3);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
