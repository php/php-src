--TEST--
Test pow() - wrong params test pow()
--INI--
precision=14
--FILE--
<?php
pow();
pow(36);
pow(36,4,true);
?>
--EXPECTF--

Warning: pow() expects exactly 2 parameters, 0 given in %s line 2

Warning: pow() expects exactly 2 parameters, 1 given in %s line 3

Warning: pow() expects exactly 2 parameters, 3 given in %s line 4


