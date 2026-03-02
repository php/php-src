--TEST--
Interface constants can be overridden directly
--FILE--
<?php

interface I
{
    const X = 1;
}

class C implements I
{
    const X = 2;
}

?>
--EXPECT--
