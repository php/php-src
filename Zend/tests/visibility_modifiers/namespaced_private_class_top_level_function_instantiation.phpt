--TEST--
Visibility modifiers: Namespaced private class, top level function instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    function c()
    {
        $b = new A\B();

        echo 'Should not work';
    }

    c();
}

?>
--EXPECTF--
Fatal error: Function c cannot instantiate package private class A\B in %s on line %d
