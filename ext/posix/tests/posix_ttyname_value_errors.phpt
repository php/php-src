--TEST--
posix_ttyname(): errors for invalid file descriptors
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip C int is same size as zend_long');
?>
--FILE--
<?php

$values = [
    -1,
    2**50+1,
];

foreach ($values as $value) {
    var_dump(posix_ttyname($value));
    var_dump(posix_strerror(posix_get_last_error()));
}
?>
--EXPECTF--
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be between 0 and %d in %s on line %d
bool(false)
string(19) "Bad file descriptor"

Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be between 0 and %d in %s on line %d
bool(false)
string(19) "Bad file descriptor"
