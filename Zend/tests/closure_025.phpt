--TEST--
Closure 025: Using closure in create_function()
--FILE--
<?php

$a = create_function('$x', 'return function($y) use ($x) { return $x * $y; };');

var_dump($a(2)->__invoke(4));

?>
--EXPECT--
int(8)
