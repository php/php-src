--TEST--
Interface constants can be overridden indirectly
--FILE--
<?php

interface I
{
    const X = 1;
}

class C implements I {}

class D extends C
{
    const X = 2;
}

?>
--EXPECT--
