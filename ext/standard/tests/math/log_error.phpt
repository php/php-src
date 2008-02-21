--TEST--
Test log() - wrong params test log()
--INI--
precision=14
--FILE--
<?php
log();
log(36,4,true);
log(36, -4);
?>
--EXPECTF--
Warning: log() expects at least 1 parameter, 0 given in %s on line %d

Warning: log() expects at most 2 parameters, 3 given in %s on line %d

Warning: log(): base must be greater than 0 in %s on line %d
