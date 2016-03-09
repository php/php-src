--TEST--
Test typed properties int must be allowed to widen to float
--FILE--
<?php
class Foo {

	public int $bar = 1;
}

$foo = new Foo();
$foo->bar = 2.2;

var_dump($foo->bar);
?>
--EXPECT--
float(2.2)





