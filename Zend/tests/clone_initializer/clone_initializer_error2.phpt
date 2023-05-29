--TEST--
Test that the property initializer list cannot contain invalid identifiers when using the : syntax
--FILE--
<?php

class Foo
{
    public function withProperties()
    {
        return clone $this with [1: "value"];
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token ":" in %s on line %d
