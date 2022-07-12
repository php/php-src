--TEST--
Overwritten asymmetric property
--FILE--
<?php

class A {
	public private(set) string $foo;

	public function setFoo() {
		$this->foo = 'foo';
	}
}

class B extends A {
	public private(set) string $foo;
}

$b = new B();
$b->setFoo('foo');
echo $b->foo, "\n";

?>
--EXPECT--
foo
