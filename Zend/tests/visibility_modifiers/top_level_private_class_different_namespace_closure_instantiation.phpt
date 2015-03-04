--TEST--
Visibility modifiers: Top level private class, different namespace closure instantiation
--FILE--
<?php

namespace 
{
    private class A {}
}

namespace B
{
    $c = function () {
        $a = new \A();

        echo 'Should not work';
    };

    $c();
}

?>
--EXPECTF--
Fatal error: Function B\{closure} cannot instantiate package private class A in %s on line %d
