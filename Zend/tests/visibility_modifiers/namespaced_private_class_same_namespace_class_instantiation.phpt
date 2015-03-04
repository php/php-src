--TEST--
Visibility modifiers: Namespaced private class, same namespace class instantiation
--FILE--
<?php

namespace A
{
    private class B {}

    class C
    {
        public function __construct()
        {
            $b = new B();

            echo 'Should work';
        }
    }
}

namespace
{
    $c = new \A\C();
}

?>
--EXPECTF--
Should work
