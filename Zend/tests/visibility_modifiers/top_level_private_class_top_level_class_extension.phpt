--TEST--
Visibility modifiers: Top level private class, top level class extension
--FILE--
<?php

private class A {}

class B extends A
{
    public function __construct()
    {
        echo 'Should work';
    }
}

$b = new B();

?>
--EXPECTF--
Should work
