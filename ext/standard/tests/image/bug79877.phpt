--TEST--
Bug #79877 (getimagesize function silently truncates after a null byte)
--FILE--
<?php
var_dump(getimagesize("/tmp/a.png\0xx"));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: getimagesize(): Argument #1 ($filename) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): getimagesize('/tmp/a.png\x00xx')
#1 {main}
  thrown in %s on line %d
