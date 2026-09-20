--TEST--
OSS-Fuzz #538730793 (Assertion failure when returning by-ref from closure invoke)
--FILE--
<?php

$x = function&(){};
$y = $x->__invoke();
var_dump($y);

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
NULL
