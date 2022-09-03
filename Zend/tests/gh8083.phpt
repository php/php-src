--TEST--
GH-8083 (var_dump() on closure with static variable segfaults)
--FILE--
<?php

function func() {
    static $i;
}

$x = func(...);

var_dump($x);

?>
--EXPECT--
object(Closure)#1 (2) {
  ["function"]=>
  string(4) "func"
  ["static"]=>
  array(1) {
    ["i"]=>
    NULL
  }
}
