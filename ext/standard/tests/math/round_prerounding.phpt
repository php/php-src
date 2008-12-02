--TEST--
round() prerounds results to precision
--INI--
precision=14
--FILE--
<?php
var_dump (round (0.285, 2, PHP_ROUND_HALF_UP));
?>
--EXPECT--
float(0.29)
