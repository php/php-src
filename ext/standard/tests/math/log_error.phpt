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

Warning: Wrong parameter count for log() in %s on line 2

Warning: Wrong parameter count for log() in %s on line 3

Warning: log(): base must be greater than 0 in %s on line 4
