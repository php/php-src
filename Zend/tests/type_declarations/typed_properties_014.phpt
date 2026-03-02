--TEST--
Test typed properties disallow incorrect type initial value (array)
--FILE--
<?php
class Foo {
    public array $bar = 32;
}
?>
--EXPECTF--
Fatal error: Cannot use int as default value for property Foo::$bar of type array in %s on line 3
