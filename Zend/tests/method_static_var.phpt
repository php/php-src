--TEST--
Initial value of static var in method depends on the include time of the class definition
--XFAIL--
Maybe not a bug
--FILE--
<?php
class Foo {
 public function __construct() {
  eval("class Bar extends Foo {}");
  Some::foo($this);
 }
 public static function test() {
  static $i = 0;
  var_dump(++$i);
 }
}
class Some {
 public static function foo(Foo $foo) {
 }
}
foo::test();
new Foo;
foo::test();

/** 
 * function_add_ref() makes a clone of static variables for inherited functions, so $i in Bar::test gets initial value 1
 */ 
Bar::test();
Bar::test();
--EXPECT--
int(1)
int(2)
int(1)
int(2)
