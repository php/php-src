--TEST--
Test function posix_setegid() by calling it with its expected arguments
--EXTENSIONS--
posix
--FILE--
<?php
var_dump(posix_setegid(posix_getegid()));
?>
--EXPECT--
bool(true)
