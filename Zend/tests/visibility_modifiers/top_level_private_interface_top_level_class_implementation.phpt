--TEST--
Visibility modifiers: Top level private interface, top level class implementation
--FILE--
<?php

private interface A {}

class B implements A
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
