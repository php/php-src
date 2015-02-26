--TEST--
Visibility modifiers: Namespaced private class, different namespace clone instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    $privateClass = new \ReflectionClass('A\B');
    $privateClass->setAccessible(true);

    $b = $privateClass->newInstance();

    $b_cloned = clone $b;

    echo 'Should not work';
}

?>
--EXPECTF--
Fatal error: Class C\D cannot instantiate package private class A\B in %s on line %d
