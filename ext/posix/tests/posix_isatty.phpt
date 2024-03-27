--TEST--
posix_isatty(): Basic tests
--EXTENSIONS--
posix
--FILE--
<?php

var_dump(posix_isatty(STDIN));

?>
--EXPECT--
bool(false)
