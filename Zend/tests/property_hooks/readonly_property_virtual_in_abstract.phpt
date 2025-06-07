--TEST--
Virtual readonly property in interface throws
--FILE--
<?php

abstract class Test {
    public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Hooked virtual properties cannot be readonly in %s on line %d
