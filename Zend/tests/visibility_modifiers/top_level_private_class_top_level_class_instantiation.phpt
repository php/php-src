--TEST--
Visibility modifiers: Top level private class, top level class instantiation
--FILE--
<?php

private class A {}

class B
{
    public function __construct()
    {
        $a = new A();

        echo 'Should work';
    }
}

$b = new B();

?>
--EXPECTF--
Should work
