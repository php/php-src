--TEST--
Test ReflectionProperty::isReadable() with a friend
--FILE--
<?php

class A {
    friend C;

    public $publicProp;
    protected $protectedProp;
    private $privateProp;
}

class C {}

var_dump(new ReflectionProperty('A', 'publicProp')->isReadable('C', null));
var_dump(new ReflectionProperty('A', 'protectedProp')->isReadable('C', null));
var_dump(new ReflectionProperty('A', 'privateProp')->isReadable('C', null));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
