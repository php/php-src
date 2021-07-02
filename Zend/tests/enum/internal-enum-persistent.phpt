--TEST--
Internal enums are persistent
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ZendTestStringBacked::FIRST);
var_dump(ZendTestStringBacked::FIRST == ZendTestStringBacked::getFirst());
var_dump(ZendTestStringBacked::FIRST === ZendTestStringBacked::getFirst());
var_dump(ZendTestStringBacked::FIRST !== ZendTestUnbacked::FIRST);
var_dump(ZendTestStringBacked::FIRST->value);
var_dump(ZendTestIntBacked::FIRST->value);

var_dump(ZendTestStringBacked::FIRST instanceof UnitEnum, ZendTestStringBacked::FIRST instanceof BackedEnum);
var_dump(ZendTestUnbacked::FIRST instanceof UnitEnum, ZendTestUnbacked::FIRST instanceof BackedEnum);

var_dump(ZendTestUnbacked::cases());
var_dump(ZendTestStringBacked::cases());
var_dump(ZendTestIntBacked::from(42));
var_dump(ZendTestStringBacked::tryFrom("foo"));
var_dump(ZendTestStringBacked::tryFrom("a"));

?>
--EXPECT--
enum(ZendTestStringBacked::FIRST)
bool(true)
bool(true)
bool(true)
string(1) "a"
int(42)
bool(true)
bool(true)
bool(true)
bool(false)
array(2) {
  [0]=>
  enum(ZendTestUnbacked::FIRST)
  [1]=>
  enum(ZendTestUnbacked::SECOND)
}
array(2) {
  [0]=>
  enum(ZendTestStringBacked::FIRST)
  [1]=>
  enum(ZendTestStringBacked::SECOND)
}
enum(ZendTestIntBacked::FIRST)
NULL
enum(ZendTestStringBacked::FIRST)
