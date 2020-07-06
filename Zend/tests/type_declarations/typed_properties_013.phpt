--TEST--
Test typed properties disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
    public int $bar = "string";
}
?>
--EXPECTF--
Fatal error: Property Foo::$bar of type int cannot have a default value of type string in %s on line %d
