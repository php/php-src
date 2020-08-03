--TEST--
Bug #79877 (getimagesize function silently truncates after a null byte)
--FILE--
<?php
var_dump(getimagesize("/tmp/a.png\0xx"));
?>
--EXPECTF--
Warning: getimagesize(): Invalid path in %s on line %d
NULL
