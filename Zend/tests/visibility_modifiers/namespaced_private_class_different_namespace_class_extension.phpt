--TEST--
Visibility modifiers: Namespaced private class, different namespace class extension
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace C
{
    class D extends \A\B
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
Fatal error: Class C\D cannot extend package private class A\B in %s on line %d
