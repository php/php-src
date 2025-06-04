--TEST--
Virtual readonly property in interface throws
--FILE--
<?php

interface Test {
    public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Hooked virtual properties cannot be readonly in %s on line %d
