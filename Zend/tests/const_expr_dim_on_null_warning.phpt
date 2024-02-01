--TEST--
DIM on null in constant expr should emit warning
--FILE--
<?php
const C = (null)['foo'];
var_dump(C);
?>
--EXPECTF--
Warning: Trying to access array offset on value of type null in %s on line %d
NULL
