--TEST--
Test hexdec() - wrong params test hexdec()
--FILE--
<?php
hexdec();
hexdec('0x123abc',true);
?>
--EXPECTF--

Warning: hexdec() expects exactly 1 parameter, 0 given in %s on line %d

Warning: hexdec() expects exactly 1 parameter, 2 given in %s on line %d
