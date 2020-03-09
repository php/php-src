--TEST--
Test that non-private final static properties can't override non-final static properties
--FILE--
<?php

class Foo
{
    protected static int $property1;
}

class Bar extends Foo
{
    final protected static int $property1;
}

?>
--EXPECTF--
Fatal error: Cannot redeclare non-final property Foo::$property1 as final Bar::$property1 in %s on line %d
