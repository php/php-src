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

Warning: Wrong parameter count for exp() in %s on line 2

Warning: Wrong parameter count for exp() in %s on line 3
