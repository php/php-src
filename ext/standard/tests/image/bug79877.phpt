--TEST--
Bug #79877 (getimagesize function silently truncates after a null byte)
--FILE--
<?php
var_dump(getimagesize("/tmp/a.png\0xx"));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: getimagesize(): Argument #1 ($image_path) must not contain any null bytes in %s:%d
Stack trace:
%a
