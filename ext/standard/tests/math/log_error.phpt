--TEST--
Test log() - wrong params test log()
--INI--
precision=14
--FILE--
<?php
log(36, -4);
?>
--EXPECTF--
Warning: log(): base must be greater than 0 in %s on line %d
