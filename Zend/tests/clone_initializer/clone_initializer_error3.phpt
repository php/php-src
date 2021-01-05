--TEST--
Test that the property initializer list can only contain literals
--FILE--
<?php

class Foo
{
    public function withProperties()
    {
        $property = "string";

        return clone $this with {$property: "value"};
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$property" in %s on line %d
