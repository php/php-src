--TEST--
Visibility modifiers: Top level private class, top level instantiation
--FILE--
<?php

private class A {}

$a = new A();

echo 'Should work';

?>
--EXPECTF--
Should work
