--TEST--
Test that non-private final properties can't override non-final properties
--FILE--
<?php

class Foo
{
    protected int $property1;
}

class Bar extends Foo
{
    final protected int $property1;
}

?>
--EXPECTF--
Fatal error: Cannot redeclare non-final property Foo::$property1 as final Bar::$property1 in %s on line %d
