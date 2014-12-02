--TEST--
Visibility modifiers: Namespaced private class, top level class instantiation
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    class C
    {
        public function __construct()
        {
            $b = new A\B();

            echo 'Should work';
        }
    }

    $c = new C();
}

?>
--EXPECTF--
Fatal error: Class C cannot instantiate package private class A\B in %s on line %d
