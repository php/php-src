--TEST--
__sleep cannot take arguments
--FILE--
<?php
class Foo {
    public function __sleep(string $name) {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__sleep() cannot take arguments in %s on line %d
