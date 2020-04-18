--TEST--
Bug #63111 (is_callable() lies for abstract static method)
--FILE--
<?php
abstract class Foo {
        abstract static function bar();
}
interface MyInterface {
    static function bar();
}
abstract class Bar {
    static function foo() {
        echo "ok\n";
    }
}
var_dump(is_callable(array("Foo", "bar")));
var_dump(is_callable("Foo::bar"));
var_dump(is_callable(array("MyInterface", "bar")));
var_dump(is_callable("MyInterface::bar"));
var_dump(is_callable(array("Bar", "foo")));
var_dump(is_callable("Bar::foo"));
Bar::foo();
Foo::bar();
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
ok

Fatal error: Uncaught Error: Cannot call abstract method Foo::bar() in %sbug63111.php:20
Stack trace:
#0 {main}
  thrown in %sbug63111.php on line 20
