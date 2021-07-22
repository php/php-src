--TEST--
Internal enums
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump($bar = ZendTestUnitEnum::Bar);
var_dump($bar === ZendTestUnitEnum::Bar);

var_dump($foo = zend_get_unit_enum());
var_dump($foo === ZendTestUnitEnum::Foo);

var_dump(ZendTestUnitEnum::cases());

var_dump(ZendTestStringEnum::Foo);
var_dump(ZendTestStringEnum::Foo->value);
var_dump(ZendTestStringEnum::from("Test2"));
var_dump(ZendTestStringEnum::cases());

?>
--EXPECT--
enum(ZendTestUnitEnum::Bar)
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
string(5) "Test1"
enum(ZendTestStringEnum::Bar)
array(2) {
  [0]=>
  enum(ZendTestStringEnum::Foo)
  [1]=>
  enum(ZendTestStringEnum::Bar)
}
