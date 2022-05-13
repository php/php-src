--TEST--
Internal enums
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump($bar = ZendTestUnitEnum::Bar);
var_dump($bar === ZendTestUnitEnum::Bar);
var_dump($bar instanceof UnitEnum);

var_dump($foo = zend_get_unit_enum());
var_dump($foo === ZendTestUnitEnum::Foo);

var_dump(ZendTestUnitEnum::cases());
echo "\n";

var_dump($foo = ZendTestStringEnum::Foo);
var_dump($foo instanceof BackedEnum);
var_dump(ZendTestStringEnum::Foo->value);
var_dump($bar = ZendTestStringEnum::from("Test2"));
var_dump($bar === ZendTestStringEnum::Bar);
var_dump(ZendTestStringEnum::tryFrom("Test3"));
var_dump(ZendTestStringEnum::cases());

var_dump($s = serialize($foo));
var_dump(unserialize($s));
var_dump(unserialize($s) === $foo);

?>
--EXPECT--
enum(ZendTestUnitEnum::Bar)
bool(true)
bool(true)
enum(ZendTestUnitEnum::Foo)
bool(true)
array(2) {
  [0]=>
  enum(ZendTestUnitEnum::Foo)
  [1]=>
  enum(ZendTestUnitEnum::Bar)
}

enum(ZendTestStringEnum::Foo)
bool(true)
string(5) "Test1"
enum(ZendTestStringEnum::Bar)
bool(true)
NULL
array(3) {
  [0]=>
  enum(ZendTestStringEnum::Foo)
  [1]=>
  enum(ZendTestStringEnum::Bar)
  [2]=>
  enum(ZendTestStringEnum::Baz)
}
string(30) "E:22:"ZendTestStringEnum:Foo";"
enum(ZendTestStringEnum::Foo)
bool(true)
