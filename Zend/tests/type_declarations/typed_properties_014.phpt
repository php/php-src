--TEST--
Test typed properties disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
    public array $bar = 32;
}
?>
--EXPECTF--
Fatal error: Property int::$Foo of type bar cannot have a default value of type array in %s on line %d
