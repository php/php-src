--TEST--
posix_setpgid() test
--CREDITS--
Michael Paul da Rosa <michael [at] michaelpaul [dot] com [dot] br>
phpdublin #phptestfest
--SKIPIF--
<?php if (!extension_loaded('posix')) echo 'skip'; ?>
--FILE--
<?php
// success path
var_dump(posix_setpgid(posix_getpid(), posix_getpgid(posix_getpid())));
// error path
var_dump(posix_setpgid(-1, -1));
?>
--EXPECT--
bool(true)
bool(false)
