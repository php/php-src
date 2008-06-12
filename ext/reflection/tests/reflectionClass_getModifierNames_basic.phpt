--TEST--
ReflectionClass::getModifierNames() basic tests
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php

class a {}
abstract class b {}
final class c {}

class x
{
	function __construct() {}
	function __destruct() {}
	private function a() {}
	private static function b() {}
	protected function c() {}
	protected static function d() {}
	public function e() {}
	public static function f() {}
	final function g() {}
	function h() {}
}

abstract class y
{
	abstract function a();
	abstract protected function b();
}

function dump_modifierNames($class) {
	$obj = new ReflectionClass($class);
	var_dump($obj->getName(), Reflection::getModifierNames($obj->getModifiers()));
}

function dump_methodModifierNames($class) {
	$obj = new ReflectionClass($class);
	foreach($obj->getMethods() as $method) {
		var_dump($obj->getName() . "::" . $method->getName(), Reflection::getModifierNames($method->getModifiers()));
	}
}

dump_modifierNames('a');
dump_modifierNames('b');
dump_modifierNames('c');

dump_methodModifierNames('x');
dump_methodModifierNames('y');

?>
==DONE==
--EXPECT--
unicode(1) "a"
array(0) {
}
unicode(1) "b"
array(1) {
  [0]=>
  unicode(8) "abstract"
}
unicode(1) "c"
array(1) {
  [0]=>
  unicode(5) "final"
}
unicode(14) "x::__construct"
array(1) {
  [0]=>
  unicode(6) "public"
}
unicode(13) "x::__destruct"
array(1) {
  [0]=>
  unicode(6) "public"
}
unicode(4) "x::a"
array(1) {
  [0]=>
  unicode(7) "private"
}
unicode(4) "x::b"
array(2) {
  [0]=>
  unicode(7) "private"
  [1]=>
  unicode(6) "static"
}
unicode(4) "x::c"
array(1) {
  [0]=>
  unicode(9) "protected"
}
unicode(4) "x::d"
array(2) {
  [0]=>
  unicode(9) "protected"
  [1]=>
  unicode(6) "static"
}
unicode(4) "x::e"
array(1) {
  [0]=>
  unicode(6) "public"
}
unicode(4) "x::f"
array(2) {
  [0]=>
  unicode(6) "public"
  [1]=>
  unicode(6) "static"
}
unicode(4) "x::g"
array(2) {
  [0]=>
  unicode(5) "final"
  [1]=>
  unicode(6) "public"
}
unicode(4) "x::h"
array(1) {
  [0]=>
  unicode(6) "public"
}
unicode(4) "y::a"
array(2) {
  [0]=>
  unicode(8) "abstract"
  [1]=>
  unicode(6) "public"
}
unicode(4) "y::b"
array(2) {
  [0]=>
  unicode(8) "abstract"
  [1]=>
  unicode(9) "protected"
}
==DONE==
