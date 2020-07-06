--TEST--
Test typed properties disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
    public array $bar = 32;
}
?>
--EXPECTF--
Fatal error: Property Foo::$bar of type array cannot have a default value of type int in %s on line %d
