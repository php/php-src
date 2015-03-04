--TEST--
Visibility modifiers: Top level private trait, top level class usage
--FILE--
<?php

private trait A {}

class B
{
    use A;

    public function __construct()
    {
        echo 'Should work';
    }
}

$b = new B();

?>
--EXPECTF--
Should work
