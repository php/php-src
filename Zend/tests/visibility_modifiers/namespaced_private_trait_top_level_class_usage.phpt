--TEST--
Visibility modifiers: Namespaced private trait, top level class usage
--FILE--
<?php

namespace A
{
    private trait B {}
}

namespace
{
    class C
    {
        use \A\B;

        public function __construct()
        {
            echo 'Should work';
        }
    }

    $c = new C();
}

?>
--EXPECTF--
Fatal error: Class C cannot use package private trait A\B in %s on line %d
