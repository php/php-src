--TEST--
posix_ttyname(): invalid argument type does not fall back to file descriptor 0
--EXTENSIONS--
posix
--FILE--
<?php

var_dump(posix_ttyname("abc"));
var_dump(posix_ttyname([]));
var_dump(posix_ttyname(new stdClass));
var_dump(posix_isatty("abc"));
?>
--EXPECTF--
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, string given in %s on line %d
bool(false)

Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, array given in %s on line %d
bool(false)

Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, stdClass given in %s on line %d
bool(false)

Warning: posix_isatty(): Argument #1 ($file_descriptor) must be of type int|resource, string given in %s on line %d
bool(false)
