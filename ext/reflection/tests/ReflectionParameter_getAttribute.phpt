--TEST--
ReflectionParameter::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

function myFunc(#[Foo, Bar, Foo] int $a, int $b) {}

$params = (new ReflectionFunction('myFunc'))->getParameters();

$rp = $params[0];
var_dump($rp->getAttribute('Foo')?->getName());
var_dump($rp->getAttribute()?->getName());
var_dump($rp->getAttribute('Baz'));

$rp2 = $params[1];
var_dump($rp2->getAttribute());

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
NULL
