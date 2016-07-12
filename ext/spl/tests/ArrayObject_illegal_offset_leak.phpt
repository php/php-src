--TEST--
Assignments to illegal ArrayObject offsets shouldn't leak
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
$ao[[]] = new stdClass;

?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d
