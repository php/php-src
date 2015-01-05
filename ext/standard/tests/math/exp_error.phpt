--TEST--
Test exp() - wrong params for exp()
--INI--
precision=14
--FILE--
<?php
exp();
exp(23,true);
?>
--EXPECTF--

Warning: exp() expects exactly 1 parameter, 0 given in %s on line %d

Warning: exp() expects exactly 1 parameter, 2 given in %s on line %d
