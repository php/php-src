--TEST--
Bug #70804 (Unary add on negative zero produces positive zero)
--FILE--
<?php

var_dump(+(-0.0));
var_dump(+(float)"-0");

$foo = +(-sin(0));

var_dump($foo);

?>
--EXPECT--
float(-0)
float(-0)
float(-0)
