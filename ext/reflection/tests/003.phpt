--TEST--
ReflectionMethod::invoke() with base class method
--FILE--
<?php

class Foo
{
    function Test()
    {
        echo __METHOD__ . "\n";
    }
}

class Bar extends Foo
{
    function Test()
    {
        echo __METHOD__ . "\n";
    }
}

$o = new Bar;
$r = new ReflectionMethod('Foo','Test');

$r->invoke($o);

?>
--EXPECT--
Foo::Test
