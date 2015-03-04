--TEST--
Visibility modifiers: Top level private trait, different namespace class usage
--FILE--
<?php

namespace 
{
    private trait A {}
}

namespace B
{
    class C
    {
        use \A;

        public function __construct()
        {
            echo 'Should work';
        }
    }
}

namespace 
{
    $b = new B\C();
}

?>
--EXPECTF--
Fatal error: Class B\C cannot use package private trait A in %s on line %d
