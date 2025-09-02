--TEST--
Overwritten protected asymmetric property with public property
--FILE--
<?php

class A {
	public protected(set) string $foo;
}

class B extends A {
	public string $foo;
}

$b = new B();
$b->foo = 'foo';
echo $b->foo, "\n";

?>
--EXPECT--
foo
