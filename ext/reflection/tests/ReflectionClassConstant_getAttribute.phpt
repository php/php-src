--TEST--
ReflectionClassConstant::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

class MyClass {
    #[Foo, Bar, Foo]
    const MY_CONST = 42;

    const PLAIN = 0;
}

$rcc = new ReflectionClassConstant('MyClass', 'MY_CONST');
var_dump($rcc->getAttribute('Foo')?->getName());
var_dump($rcc->getAttribute()?->getName());
var_dump($rcc->getAttribute('Baz'));

$rcc2 = new ReflectionClassConstant('MyClass', 'PLAIN');
var_dump($rcc2->getAttribute());

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
NULL
