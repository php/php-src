--TEST--
ReflectionClass::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

#[Foo, Bar, Foo]
class MyClass {}

$rc = new ReflectionClass('MyClass');

var_dump($rc->getAttribute('Foo')?->getName());
var_dump($rc->getAttribute()?->getName());
var_dump($rc->getAttribute('Baz'));

// No attributes
class Plain {}
$rc2 = new ReflectionClass('Plain');
var_dump($rc2->getAttribute());

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
NULL
