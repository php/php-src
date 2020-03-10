--TEST--
Test that final properties of composite types can be accessed for reading
--XFAIL--
Object properties are fetched with incorrect access mode
--FILE--
<?php

class Foo
{
    final public array $property1 = ["foo", "bar"];
    final public stdClass $property2;

    final public static array $property3 = ["foo", "bar"];
    final public static stdClass $property4;
}

$foo = new Foo();
$foo->property2 = new stdClass();
Foo::$property4 = new stdClass();

echo "Object properties:\n";
var_dump($foo->property1);
var_dump($foo->property1[0]);

var_dump($foo->property2);
$foo->property2->foo = "foo";
var_dump($foo->property2->foo);
var_dump($foo->property2->bar);

echo "\nClass properties:\n";
var_dump(Foo::$property3);
var_dump(Foo::$property3[0]);

var_dump(Foo::$property4);
Foo::$property4->foo = "foo";
var_dump(Foo::$property4->foo);
var_dump(Foo::$property4->bar);

?>
--EXPECTF--
Object properties:
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
string(3) "foo"
object(stdClass)#2 (0) {
}
string(3) "foo"

Warning: Undefined property: stdClass::$bar in %s on line %d
NULL

Class properties:
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
string(3) "foo"
object(stdClass)#3 (0) {
}
string(3) "foo"

Warning: Undefined property: stdClass::$bar in %s on line %d
NULL
