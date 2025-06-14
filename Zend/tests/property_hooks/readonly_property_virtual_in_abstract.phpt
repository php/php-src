--TEST--
Hooked properties in abstract classes cannot be readonly
--FILE--
<?php

abstract class Test {
    public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Hooked properties in abstract classes may not be declared readonly in %s on line %d
