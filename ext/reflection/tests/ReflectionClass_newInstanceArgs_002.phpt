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
Fatal error: Uncaught TypeError: ReflectionClass::newInstanceArgs(): Argument #1 ($args) must be of type array, string given in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstanceArgs('x')
#1 {main}
  thrown in %s on line %d
