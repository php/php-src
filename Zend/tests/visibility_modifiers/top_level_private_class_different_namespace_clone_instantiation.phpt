--TEST--
Visibility modifiers: Top level private class, different namespace clone instantiation
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    $privateClass = new \ReflectionClass('A');
    $privateClass->setAccessible(true);

    $a = $privateClass->newInstance();

    $a_cloned = clone $a;

    echo 'Should not work';
}

?>
--EXPECTF--
Fatal error: Class B\C cannot instantiate package private class A in %s on line %d
