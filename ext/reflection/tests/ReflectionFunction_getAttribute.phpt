--TEST--
ReflectionFunction::getAttribute() and ReflectionMethod::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

#[Foo, Bar, Foo]
function myFunc() {}

$rf = new ReflectionFunction('myFunc');

// Returns first match when name given
var_dump($rf->getAttribute('Foo')?->getName());

// Returns first match when name absent
var_dump($rf->getAttribute()?->getName());

// Returns null when no match
var_dump($rf->getAttribute('Baz'));

// Method
class MyClass {
    #[Foo, Bar]
    public function myMethod() {}
}

$rm = new ReflectionMethod('MyClass', 'myMethod');
var_dump($rm->getAttribute('Bar')?->getName());
var_dump($rm->getAttribute('Baz'));

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
string(3) "Bar"
NULL
