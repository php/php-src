--TEST--
Visibility modifiers: Top level private class, top level clone instantiation
--FILE--
<?php

private class A {}

$a = new A();

$a_cloned = clone $a;

echo 'Should work';

?>
--EXPECTF--
Should work
