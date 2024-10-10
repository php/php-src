--TEST--
Check interaction of first-class callables with optimization
--FILE--
<?php

function test1() {}
function test2() { return 42; }

var_dump(test1(...));
var_dump(test2(...));

?>
--EXPECT--
object(Closure)#1 (1) {
  ["function"]=>
  string(5) "test1"
}
object(Closure)#1 (1) {
  ["function"]=>
  string(5) "test2"
}
