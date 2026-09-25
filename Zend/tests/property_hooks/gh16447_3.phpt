--TEST--
GH-16447: Enum error messages should not suggest adding methods ($name get allowed)
--FILE--
<?php

interface I {
    public string $name { get; }
}

enum E implements I {}

?>
OKAY
--EXPECT--
OKAY
