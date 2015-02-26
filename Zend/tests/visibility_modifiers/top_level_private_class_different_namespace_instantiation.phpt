--TEST--
Visibility modifiers: Top level private class, different namespace instantiation
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    $a = new \A();

    echo 'Should not work';
}

?>
--EXPECTF--
Fatal error: Should not work
