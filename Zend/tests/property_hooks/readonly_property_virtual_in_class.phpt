--TEST--
Virtual readonly property in class throws
--FILE--
<?php

class Test {
    public readonly int $prop {
        get => 42;
    }
}
?>
--EXPECTF--
Fatal error: Hooked virtual properties cannot be declared readonly in %s on line %d
