--TEST--
Test that the property initializer list is required when "with" is given
--FILE--
<?php

class Foo
{
    public function withProperties()
    {
        return clone $this with;
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token ";", expecting "{" in %s on line %d
