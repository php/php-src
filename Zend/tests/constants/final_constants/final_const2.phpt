--TEST--
Final class constants cannot be overridden
--FILE--
<?php

class Foo
{
    final const A = "foo";
}

class Bar extends Foo
{
    const A = "bar";
}

?>
--EXPECTF--
Fatal error: Bar::A cannot override final constant Foo::A in %s on line %d
