--TEST--
Test posix_sysconf
--EXTENSIONS--
posix
--FILE--
<?php
var_dump(posix_sysconf(-1));
var_dump(posix_errno() != 0);
var_dump(posix_sysconf(POSIX_SC_NPROCESSORS_ONLN));
?>
--EXPECTF--
int(-1)
bool(false)
int(%d)
