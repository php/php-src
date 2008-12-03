--TEST--
posix_isatty(): Basic tests
--SKIP--
<?php if (!posix_mknod('posix_isatty')) die('skip posix_isatty() not found'); ?>
--FILE--
<?php

var_dump(posix_isatty(0));

?>
--EXPECTF--
bool(%s)
