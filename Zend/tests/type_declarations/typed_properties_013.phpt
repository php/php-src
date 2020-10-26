--TEST--
Test typed properties disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
    public int $bar = "string";
}
?>
--EXPECTF--
Fatal error: Property string::$Foo of type bar cannot have a default value of type int in %s on line %d
