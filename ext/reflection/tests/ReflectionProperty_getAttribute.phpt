--TEST--
ReflectionProperty::getAttribute()
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Foo {}

#[Attribute(Attribute::TARGET_ALL | Attribute::IS_REPEATABLE)]
class Bar {}

class MyClass {
    #[Foo, Bar, Foo]
    public int $prop = 0;

    public int $plain = 0;
}

$rp = new ReflectionProperty('MyClass', 'prop');
var_dump($rp->getAttribute('Foo')?->getName());
var_dump($rp->getAttribute()?->getName());
var_dump($rp->getAttribute('Baz'));

$rp2 = new ReflectionProperty('MyClass', 'plain');
var_dump($rp2->getAttribute());

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
NULL
NULL
