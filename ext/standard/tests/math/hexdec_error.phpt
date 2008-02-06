--TEST--
Test hexdec() - wrong params test hexdec()
--FILE--
<?php
hexdec();
hexdec('0x123abc',true);
?>
--EXPECTF--

Warning: Wrong parameter count for hexdec() in %s on line 2

Warning: Wrong parameter count for hexdec() in %s on line 3
