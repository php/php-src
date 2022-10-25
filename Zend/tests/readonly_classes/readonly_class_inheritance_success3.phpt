--TEST--
Non-readonly child of a readonly class can create dynamic properties
--FILE--
<?php

readonly class Foo
{
}

class Bar extends Foo
{
}

$bar = new Bar();
$bar->baz = 1;

?>
--EXPECTF--
Deprecated: Creation of dynamic property Bar::$baz is deprecated in %s on line %d
