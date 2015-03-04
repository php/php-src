--TEST--
Visibility modifiers: Namespaced private class, same namespace clone instantiation
--FILE--
<?php

namespace A
{
    private class B {}

    $privateClass = new \ReflectionClass('A\B');
    $privateClass->setAccessible(true);

    $b = $privateClass->newInstance();

    $b_cloned = clone $b;

    echo 'Should work';
}

?>
--EXPECTF--
Should work
