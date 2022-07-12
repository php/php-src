--TEST--
Overwritten private asymmetric property with public property
--FILE--
<?php

class A {
	public private(set) string $foo;
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
