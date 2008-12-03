--TEST--
posix_getcwd(): Basic tests
--SKIP--
<?php if (!posix_mknod('posix_getcwd')) die('skip posix_getcwd() not found'); ?>
--FILE--
<?php

var_dump(posix_getcwd());
var_dump(posix_getcwd(1));

?>
--EXPECTF--
string(%d) "%s"

Warning: posix_getcwd() expects exactly 0 parameters, 1 given in %s on line %d
NULL
