--TEST--
ReflectionClass::newInstanceArgs() - wrong arg type
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {
	public function __construct($a, $b) {
		echo "In constructor of class B with arg $a\n";
	}
}
$rc = new ReflectionClass('A');
$a = $rc->newInstanceArgs('x');
var_dump($a);

?>
--EXPECTF--

Catchable fatal error: Argument 1 passed to ReflectionClass::newInstanceArgs() must be of the type array, string given in %s on line 8
