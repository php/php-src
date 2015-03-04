--TEST--
Visibility modifiers: Top level private class, different namespace class extension
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    class C extends \A
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
Fatal error: Class B\C cannot extend package private class A in %s on line %d
