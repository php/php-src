--TEST--
Visibility modifiers: Namespaced private interface, top level class implementation
--FILE--
<?php

namespace A
{
    private interface B {}
}

namespace
{
    class C implements \A\B
    {
        public function __construct()
        {
            echo 'Should work';
        }
    }

    $c = new C();
}

?>
--EXPECTF--
Fatal error: Class C cannot implement package private interface A\B in %s on line %d
