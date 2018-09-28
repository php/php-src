--TEST--
Using parent does not create compile-time warnings
--FILE--
<?php

trait P {
	public function m(parent $arg): parent {
		return $arg;
	}
}

class A {}

class B extends A {
	use P;
}

$b = new B();
$a = $b->m(new A());

print "OK\n";

?>
--EXPECT--
OK

