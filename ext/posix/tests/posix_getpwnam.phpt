--TEST--
posix_getpwnam(): Basic tests
--SKIP--
<?php if (!function_exists('posix_getpwnam')) die('skip posix_getpwnam() not found'); ?>
--FILE--
<?php

var_dump(posix_getpwnam(1));
var_dump(posix_getpwnam(''));
var_dump(posix_getpwnam(NULL));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
