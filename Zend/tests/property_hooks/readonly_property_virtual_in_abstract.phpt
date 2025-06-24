--TEST--
Virtual readonly property in abstract class triggers non-abstract body error
--FILE--
<?php

abstract class Test {
    public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Non-abstract property hook must have a body in %s on line %d
