--TEST--
Bug #37816 (ReflectionProperty does not throw exception when accessing protected attribute)
--FILE--
<?php

class TestClass
{
    protected $p = 2;
}

$o = new TestClass;

$r = new ReflectionProperty($o, 'p');

var_dump($r->getValue($o));

?>
--EXPECT--
int(2)
