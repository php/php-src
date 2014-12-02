--TEST--
Visibility modifiers: Namespaced private trait, different namespace class usage
--FILE--
<?php

namespace A
{
    private trait B {}
}

namespace C
{
    class D
    {
        use \A\B;

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
Fatal error: Class C\D cannot use package private trait A\B in %s on line %d
