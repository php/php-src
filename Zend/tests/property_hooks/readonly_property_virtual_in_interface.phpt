--TEST--
Interface properties cannot be readonly
--FILE--
<?php

interface Test {
    public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Interface properties may not be declared readonly in %s on line %d
