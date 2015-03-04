--TEST--
Visibility modifiers: Namespaced private interface, same namespace class implementation
--FILE--
<?php

namespace A
{
    private interface B {}

    class C implements B
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
