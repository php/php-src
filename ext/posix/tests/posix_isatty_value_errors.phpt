--TEST--
posix_isatty(): errors for invalid file descriptors
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
    10024,
    2**50+1,
];

foreach ($values as $value) {
    var_dump(posix_isatty($value));
    var_dump(posix_strerror(posix_get_last_error()));
}
?>
--EXPECT--
bool(false)
string(19) "Bad file descriptor"
bool(false)
string(19) "Bad file descriptor"
bool(false)
string(19) "Bad file descriptor"
