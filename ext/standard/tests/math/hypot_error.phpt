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


Warning: Wrong parameter count for hypot() in %s on line 2

Warning: Wrong parameter count for hypot() in %s on line 3

Warning: Wrong parameter count for hypot() in %s on line 4
