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

try
{
    $x = $r->getValue($o);
}
catch (Exception $e)
{
    echo 'Caught: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
Caught: Cannot access non-public property TestClass::$p
