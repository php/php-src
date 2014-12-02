--TEST--
Visibility modifiers: Namespaced private class, same namespace class extension
--FILE--
<?php

namespace A
{
    private class B {}

    class C extends B
    {
        public function __construct()
        {
            echo 'Should work';
        }
    }
}

namespace
{
    $c = new A\C();
}

?>
--EXPECTF--
Should work
