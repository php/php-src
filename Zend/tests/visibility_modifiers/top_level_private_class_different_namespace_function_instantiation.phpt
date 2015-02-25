--TEST--
Visibility modifiers: Top level private class, different namespace function instantiation
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    function c()
    {
        $a = new \A();

        echo 'Should work';
    }
}

namespace 
{
    $b = \B\c();
}

?>
--EXPECTF--
Fatal error: Function B\c cannot instantiate package private class A in %s on line %d
