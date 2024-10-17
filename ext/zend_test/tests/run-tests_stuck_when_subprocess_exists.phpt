--TEST--
run-tests: Does not get stuck when a test spawns a subprocess and dies uncleanly.
--EXTENSIONS--
posix
--FILE--
<?php

$handle = popen("sleep 5; echo not visible", "r");

fwrite(STDERR, "foo\n");
posix_kill(posix_getpid(), 9);

?>
--EXPECTF--
foo%A
Termsig=9
