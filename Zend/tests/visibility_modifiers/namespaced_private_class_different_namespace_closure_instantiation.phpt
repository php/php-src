--TEST--
Visibility modifiers: Namespaced private class, different namespace closure instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    $d = function () {
        $b = new \A\B();

        echo 'Should not work';
    };

    $d();
}

?>
--EXPECTF--
Fatal error: Function C\{closure} cannot instantiate package private class A\B in %s on line %d
