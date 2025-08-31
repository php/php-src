--TEST--
Final interface constants cannot be overridden directly
--FILE--
<?php

interface I
{
    final public const X = 1;
}

class C implements I
{
    const X = 2;
}

?>
--EXPECTF--
Fatal error: C::X cannot override final constant I::X in %s on line %d
