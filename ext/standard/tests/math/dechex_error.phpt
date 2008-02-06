--TEST--
Test dechex() - wrong params dechex()
--FILE--
<?php
dechex();
dechex(23,2,true);
?>

--EXPECTF--
 
Warning: dechex() expects exactly 1 parameter, 0 given in %s on line %d

Warning: dechex() expects exactly 1 parameter, 3 given in %s on line %d
