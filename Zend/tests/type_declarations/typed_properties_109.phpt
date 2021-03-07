--TEST--
Test typed properties disallow noreturn
--FILE--
<?php
class Foo {
    public noreturn $int;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Property Foo::$int cannot have type noreturn in %s on line 3
