--TEST--
Visibility modifiers: Top level private class, top level closure instantiation
--FILE--
<?php

private class A {}

$b = function () {
    $a = new A();

    echo 'Should work';
};

$b();

?>
--EXPECTF--
Should work
