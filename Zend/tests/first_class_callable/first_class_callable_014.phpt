--TEST--
First class callable with nullsafe method call (unrelated)
--FILE--
<?php

$foo = null;
var_dump($foo?->foo($foo->bar(...)));

?>
--EXPECT--
NULL
