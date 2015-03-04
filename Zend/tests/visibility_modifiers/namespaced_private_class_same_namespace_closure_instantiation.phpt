--TEST--
Visibility modifiers: Namespaced private class, same namespace closure instantiation
--FILE--
<?php

namespace A
{
    private class B {}

    $c = function () {
        $b = new B();

        echo 'Should work';
    };

    $c();
}

?>
--EXPECTF--
Should work
