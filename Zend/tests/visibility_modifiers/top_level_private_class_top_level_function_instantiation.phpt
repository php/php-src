--TEST--
Visibility modifiers: Top level private class, top level function instantiation
--FILE--
<?php

private class A {}

function b()
{
    $a = new A();

    echo 'Should work';
}

$b = b();

?>
--EXPECTF--
Should work
