--TEST--
Visibility modifiers: Top level private interface, different namespace class implementation
--FILE--
<?php

namespace 
{
    private interface A {}
}

namespace B
{
    class C implements \A
    {
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
Fatal error: Class B\C cannot implement package private interface A in %s on line %d
