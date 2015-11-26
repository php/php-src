--TEST--
Test atan2() - wrong params atan2()
--FILE--
<?php
atan2();
atan2(36);
atan2(36,25,0);
?>
--EXPECTF--
Warning: atan2() expects exactly 2 parameters, 0 given in %s on line %d

Warning: atan2() expects exactly 2 parameters, 1 given in %s on line %d

Warning: atan2() expects exactly 2 parameters, 3 given in %s on line %d
