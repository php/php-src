--TEST--
Fixed bug #78895 (Reflection detects abstract non-static class as abstract static).
--FILE--
<?php
abstract class Foo {
	abstract public function f1();
}
interface I {
	public function f2();
}
trait T {
	abstract public function f2();
}
class Bar extends Foo implements I {
	use T;	
	function f1() {}
	function f2() {}
}
$ref = new ReflectionClass(Foo::class);
var_dump(Reflection::getModifierNames($ref->getModifiers()));
$ref = new ReflectionClass(I::class);
var_dump(Reflection::getModifierNames($ref->getModifiers()));
$ref = new ReflectionClass(T::class);
var_dump(Reflection::getModifierNames($ref->getModifiers()));
$ref = new ReflectionClass(Bar::class);
var_dump(Reflection::getModifierNames($ref->getModifiers()));
?>
--EXPECT--
array(1) {
  [0]=>
  string(8) "abstract"
}
array(0) {
}
array(0) {
}
array(0) {
}