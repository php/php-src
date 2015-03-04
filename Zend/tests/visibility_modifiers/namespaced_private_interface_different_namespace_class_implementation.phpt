--TEST--
Visibility modifiers: Namespaced private interface, different namespace class implementation
--FILE--
<?php

namespace A
{
    private interface B {}
}

namespace C
{
    class D implements \A\B
    {
        public function __construct()
        {
            echo 'Should work';
        }
    }
}

namespace
{
    $d = new C\D();
}

?>
--EXPECTF--
Fatal error: Class C\D cannot implement package private interface A\B in %s on line %d
