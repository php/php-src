--TEST--
Test bindec() - basic function test bindec()
--FILE--
<?php
bindec();
bindec('01010101111',true);
?>
--EXPECTF--

Warning: bindec() expects exactly 1 parameter, 0 given in %s on line %d

Warning: bindec() expects exactly 1 parameter, 2 given in %s on line %d
