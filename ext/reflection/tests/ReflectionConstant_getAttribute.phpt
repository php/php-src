--TEST--
ReflectionConstant::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

#[Foo, Bar, Foo]
const MY_CONST = 42;

$rc = new ReflectionConstant('MY_CONST');
var_dump($rc->getAttribute('Foo')?->getName());
var_dump($rc->getAttribute()?->getName());
var_dump($rc->getAttribute('Baz'));

// No attributes
const PLAIN = 0;
$rc2 = new ReflectionConstant('PLAIN');
var_dump($rc2->getAttribute());

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
NULL
