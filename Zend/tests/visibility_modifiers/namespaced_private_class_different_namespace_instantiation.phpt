--TEST--
Visibility modifiers: Namespaced private class, different namespace instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    $b = new \A\B();

    echo 'Should not work';
}

?>
--EXPECTF--
Fatal error: Class C\D cannot instantiate package private class A\B in %s on line %d
