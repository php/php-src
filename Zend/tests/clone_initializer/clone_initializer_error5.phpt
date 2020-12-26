--TEST--
Test that the "clone with" doesn't support expressions as property names
--FILE--
<?php

class Foo
{
    public $bar;
}

$property = "bar";

$foo = new Foo();
$foo = clone $foo with {$property: 1};
var_dump($foo);

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$property" in %s on line %d
