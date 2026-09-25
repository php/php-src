--TEST--
GH-16447: Enum error messages should not suggest adding methods ($value get allowed on backed enum)
--FILE--
<?php

interface I {
    public string $value { get; }
}

enum E: string implements I {}

?>
OKAY
--EXPECT--
OKAY
