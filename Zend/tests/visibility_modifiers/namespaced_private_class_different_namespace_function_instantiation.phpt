--TEST--
Visibility modifiers: Namespaced private class, different namespace function instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    function d() 
    {
        $b = new \A\B();

        echo 'Should not work';
    }

    d();
}

?>
--EXPECTF--
Fatal error: Function C\d cannot instantiate package private class A\B in %s on line %d
