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
--EXPECT--
string(1) "a"
array(0) {
}
string(1) "b"
array(1) {
  [0]=>
  string(8) "abstract"
}
string(1) "c"
array(1) {
  [0]=>
  string(5) "final"
}
string(14) "x::__construct"
array(1) {
  [0]=>
  string(6) "public"
}
string(13) "x::__destruct"
array(1) {
  [0]=>
  string(6) "public"
}
string(4) "x::a"
array(1) {
  [0]=>
  string(7) "private"
}
string(4) "x::b"
array(2) {
  [0]=>
  string(7) "private"
  [1]=>
  string(6) "static"
}
string(4) "x::c"
array(1) {
  [0]=>
  string(9) "protected"
}
string(4) "x::d"
array(2) {
  [0]=>
  string(9) "protected"
  [1]=>
  string(6) "static"
}
string(4) "x::e"
array(1) {
  [0]=>
  string(6) "public"
}
string(4) "x::f"
array(2) {
  [0]=>
  string(6) "public"
  [1]=>
  string(6) "static"
}
string(4) "x::g"
array(2) {
  [0]=>
  string(5) "final"
  [1]=>
  string(6) "public"
}
string(4) "x::h"
array(1) {
  [0]=>
  string(6) "public"
}
string(4) "y::a"
array(2) {
  [0]=>
  string(8) "abstract"
  [1]=>
  string(6) "public"
}
string(4) "y::b"
array(2) {
  [0]=>
  string(8) "abstract"
  [1]=>
  string(9) "protected"
}
