--TEST--
Typed property assignment must not overwrite constants
--FILE--
<?php

class Foo {
	public float $x = 0.0;
};

$x = new Foo;
$y =& $x->x;
$y = 4;
var_dump($x, 4); /* Optimizer will merge both "4" constants, making it immediately visible */

?>
--EXPECT--
object(Foo)#1 (1) {
  ["x"]=>
  &float(4)
}
int(4)
