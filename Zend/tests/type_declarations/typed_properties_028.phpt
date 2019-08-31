--TEST--
Test typed properties respect strict types (off)
--FILE--
<?php
class Foo {
	public int $bar;
}

$foo = new Foo;
$foo->bar = "1";

var_dump($foo->bar);
?>
--EXPECT--
int(1)
