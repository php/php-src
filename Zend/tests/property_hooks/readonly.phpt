--TEST--
Hooked properties cannot be readonly
--FILE--
<?php

class Test {
    public readonly int $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Hooked properties cannot be readonly in %s on line %d
