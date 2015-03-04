--TEST--
Visibility modifiers: Namespaced private class, top level class extension
--FILE--
<?php

namespace A
{
    private class B {}
}

namespace
{
    class C extends \A\B
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
Fatal error: Class C cannot extend package private class A\B in %s on line %d
