--TEST--
Test dechex() - wrong params dechex()
--FILE--
<?php
dechex();
dechex(23,2,true);
?>

--EXPECTF--

Warning: Wrong parameter count for dechex() in %s on line 2

Warning: Wrong parameter count for dechex() in %s on line 3
