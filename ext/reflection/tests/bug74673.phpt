--TEST--
Bug #74673 (Segfault when cast Reflection object to string with undefined constant)
--FILE--
<?php

class A
{
    public function method($test = PHP_SELF + 1)
    {
    }
}

$class = new ReflectionClass('A');

echo $class;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "PHP_SELF" in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->__toString()
#1 {main}
  thrown in %s on line %d
