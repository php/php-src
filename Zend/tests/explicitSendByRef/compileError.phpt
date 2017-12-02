--TEST--
A compilation error is thrown is the ref/val mismatch is detected at compile-time
--FILE--
<?php

function argByVal($a) {}
argByVal(&$b);

?>
--EXPECTF--
Fatal error: Cannot pass reference to by-value parameter 1 in %s on line %d
