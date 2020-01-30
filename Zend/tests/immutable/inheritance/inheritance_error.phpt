--TEST--
Test that immutable classes can only be extended by immutable classes
--FILE--
<?php

immutable class Foo
{
}

immutable class Bar extends Foo
{
}

class Baz extends Bar
{
}

?>
--EXPECTF--
Fatal error: Non-immutable class Baz may not inherit from immutable class (Bar) in %s on line %d
