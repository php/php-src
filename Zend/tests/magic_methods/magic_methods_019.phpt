--TEST--
__unserialize first parameter must be an array
--FILE--
<?php
class Foo {
    public function __unserialize(string $name) {}
}
?>
--EXPECTF--
Fatal error: Foo::__unserialize(): Parameter #1 ($name) must be of type array when declared in %s on line %d
