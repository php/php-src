--TEST--
Visibility modifiers: Namespaced private trait, same namespace class usage
--FILE--
<?php

namespace A
{
    private trait B {}

    class C
    {
        use B;

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
