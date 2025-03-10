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
Fatal error: never cannot be used as a parameter type for property hooks in %s on line %d
