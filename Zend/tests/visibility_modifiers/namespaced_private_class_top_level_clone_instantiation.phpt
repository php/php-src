--TEST--
Visibility modifiers: Namespaced private class, top level clone instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    $privateClass = new \ReflectionClass('A\B');
    $privateClass->setAccessible(true);

    $b = $privateClass->newInstance();

    $b_cloned = clone $b;

    echo 'Should not work';
}

?>
--EXPECTF--
Fatal error: Class C cannot instantiate package private class A\B in %s on line %d
