--TEST--
GH-16447: Enum error messages should not suggest adding methods (set)
--FILE--
<?php

interface I {
    public string $prop { set; }
}

enum E implements I {}

?>
--EXPECTF--
Fatal error: Enum E cannot implement interface I that contains hooked property I::$prop in %s on line %d
