--TEST--
Test mt_getrandmax() - wrong paramas mt_getrandmax()
--FILE--
<?php
var_dump(mt_getrandmax(true));
?>
--EXPECTF--
Warning: mt_getrandmax() expects exactly 0 parameters, 1 given in %s on line 2
NULL
