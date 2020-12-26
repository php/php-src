--TEST--
Test that the property initializer list cannot contain integer keys
--FILE--
<?php

class Foo
{
    public function withProperties()
    {
        return clone $this with {1: "value"};
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected integer "1" in %s on line %d
