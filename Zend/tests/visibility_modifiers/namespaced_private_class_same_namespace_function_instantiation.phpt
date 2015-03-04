--TEST--
Visibility modifiers: Namespaced private class, same namespace function instantiation
--FILE--
<?php

namespace A
{
    private class B {}

    function c() {
        $b = new B();

        echo 'Should work';
    }

    c();
}

?>
--EXPECTF--
Should work
