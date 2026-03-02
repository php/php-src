--TEST--
Test typed properties disallow incorrect type initial value (scalar)
--FILE--
<?php
class Foo {
    public int $bar = "string";
}
?>
--EXPECTF--
Fatal error: Cannot use string as default value for property Foo::$bar of type int in %s on line 3
