--TEST--
Bug #31720 (Invalid object callbacks not caught in array_walk())
--FILE--
<?php
$array = array('at least one element');

array_walk($array, array($nonesuchvar,'show'));
?>
===DONE===
--EXPECTF--
Notice: Undefined variable: nonesuchvar in %s on line %d

Notice: Non-callable array passed to zend_call_function() in %s on line %d

Warning: array_walk(): Unable to call Array() - function does not exist in %s on line %d
===DONE===
