--TEST--
`never` parameter types - cannot be used for hooked properties
--FILE--
<?php

interface I
{
    public mixed $both { get; set( never $value ); }
}

?>
--EXPECTF--
Fatal error: Type of parameter $value of hook I::$both::set must be compatible with property type in %s on line %d
