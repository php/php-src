--TEST--
Overwritten private asymmetric property with public property
--FILE--
<?php

class A {
	public private(set) string $foo;
}

class B extends A {
	public protected(set) string $foo;
}

class C extends B {
	public function setFoo($foo) {
		$this->foo = $foo;
	}
}

$c = new C();
$c->setFoo('foo');
echo $c->foo, "\n";

?>
--EXPECT--
foo
