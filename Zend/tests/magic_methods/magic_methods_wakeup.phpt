--TEST--
__wakeup cannot take arguments
--FILE--
<?php
class Foo {
    public function __wakeup(string $name) {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__wakeup() cannot take arguments in %s on line %d
