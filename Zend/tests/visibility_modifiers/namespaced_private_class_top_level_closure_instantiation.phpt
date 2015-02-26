--TEST--
Visibility modifiers: Namespaced private class, top level closure instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    $c = function () {
        $b = new A\B();

        echo 'Should work';
    };

    $c();
}

?>
--EXPECTF--
Fatal error: Function {closure} cannot instantiate package private class A\B in %s on line %d
