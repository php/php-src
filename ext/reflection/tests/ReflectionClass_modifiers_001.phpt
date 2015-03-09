--TEST--
Modifiers
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
abstract class A {}
class B extends A {}
class C {}
final class D {}
interface I {}

$classes = array("A", "B", "C", "D", "I");

foreach ($classes as $class) {
	$rc = new ReflectionClass($class);
	var_dump($rc->isFinal());
	var_dump($rc->isInterface());
	var_dump($rc->isAbstract());
	var_dump($rc->getModifiers());
}
?>
--EXPECTF--
bool(false)
bool(false)
bool(true)
int(32)
bool(false)
bool(false)
bool(false)
int(0)
bool(false)
bool(false)
bool(false)
int(0)
bool(true)
bool(false)
bool(false)
int(4)
bool(false)
bool(true)
bool(false)
int(64)