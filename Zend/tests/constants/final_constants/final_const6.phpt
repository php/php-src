--TEST--
Final interface constants can be inherited
--FILE--
<?php

interface I
{
    final public const X = 1;
}

class C implements I
{
}

?>
--EXPECT--
