--TEST--
Test fmod() - wrong params test fmod()
--INI--
precision=14
--FILE--
<?php
fmod();
fmod(23);
fmod(23,2,true);
?>
--EXPECTF--

Warning: fmod() expects exactly 2 parameters, 0 given in %s on line 2

Warning: fmod() expects exactly 2 parameters, 1 given in %s on line 3

Warning: fmod() expects exactly 2 parameters, 3 given in %s on line 4
