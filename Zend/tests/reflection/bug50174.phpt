--TEST--
Bug #50174 (Incorrectly matched docComment)
--FILE--
<?php

class TestClass
{
    /** const comment */
    const C = 0;

    function x() {}
}

$rm = new ReflectionMethod('TestClass', 'x');
var_dump($rm->getDocComment());

class TestClass2
{
    /** const comment */
    const C = 0;

    public $x;
}

$rp = new ReflectionProperty('TestClass2', 'x');
var_dump($rp->getDocComment());

?>
--EXPECT--
bool(false)
bool(false)
