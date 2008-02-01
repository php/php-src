--TEST--
Test atan2() - wrong params atan2()
--FILE--
<?php
atan2();
atan2(36);
atan2(36,25,0);
?>
--EXPECTF--

Warning: Wrong parameter count for atan2() in %s on line 2

Warning: Wrong parameter count for atan2() in %s on line 3

Warning: Wrong parameter count for atan2() in %s on line 4
