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

Fatal error: Uncaught TypeError: Argument 1 passed to ReflectionClass::newInstanceArgs() must be of the type array, string given in %s:8
Stack trace:
#0 %s(%d): ReflectionClass->newInstanceArgs('x')
#1 {main}
  thrown in %s on line 8
