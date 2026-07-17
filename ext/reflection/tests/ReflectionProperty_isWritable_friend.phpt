--TEST--
Test ReflectionProperty::isWritable() with a friend
--FILE--
<?php

class A {
    friend C;

    public $publicProp;
    protected $protectedProp;
    private $privateProp;
}

class C {}

var_dump(new ReflectionProperty('A', 'publicProp')->isWritable('C', null));
var_dump(new ReflectionProperty('A', 'protectedProp')->isWritable('C', null));
var_dump(new ReflectionProperty('A', 'privateProp')->isWritable('C', null));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
