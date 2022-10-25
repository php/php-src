--TEST--
Non-readonly child of a readonly class can accept the AllowDynamicProperties attribute
--FILE--
<?php

readonly class Foo
{
}


#[AllowDynamicProperties]
class Bar extends Foo
{
}

$bar = new Bar();
$bar->baz = 1;

?>
--EXPECTF--
