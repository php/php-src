--TEST--
Visibility modifiers: Namespaced private class, different namespace class instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    class D
    {
        public function __construct()
        {
            $b = new \A\B();

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
Fatal error: Class C\D cannot instantiate package private class A\B in %s on line %d
