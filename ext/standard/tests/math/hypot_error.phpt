--TEST--
Test hypot() - wrong params test hypot()
--INI--
precision=14
--FILE--
<?php
hypot();
hypot(36);
hypot(36,25,0);
?>
--EXPECTF--

Warning: hypot() expects exactly 2 parameters, 0 given in %s on line %d

Warning: hypot() expects exactly 2 parameters, 1 given in %s on line %d

Warning: hypot() expects exactly 2 parameters, 3 given in %s on line %d
