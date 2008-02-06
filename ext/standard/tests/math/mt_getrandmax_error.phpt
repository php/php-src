--TEST--
Test mt_getrandmax() - wrong paramas mt_getrandmax()
--FILE--
<?php
var_dump(mt_getrandmax(true));
?>
--EXPECTF--
Warning: Wrong parameter count for mt_getrandmax() in %s on line 2
NULL