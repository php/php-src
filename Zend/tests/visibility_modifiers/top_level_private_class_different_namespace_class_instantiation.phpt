--TEST--
Visibility modifiers: Top level private class, different namespace class instantiation
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    class C
    {
        public function __construct()
        {
            $a = new \A();

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
Fatal error: Class B\C cannot instantiate package private class A in %s on line %d
