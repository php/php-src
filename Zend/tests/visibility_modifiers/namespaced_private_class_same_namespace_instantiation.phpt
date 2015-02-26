--TEST--
Visibility modifiers: Namespaced private class, same namespace instantiation
--FILE--
<?php

namespace A
{
    private class B {}

    $b = new B();

    echo 'Should work';
}

?>
--EXPECTF--
Should work
