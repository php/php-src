--TEST--
recursive mkdir() tests
--SKIPIF--
<?php
chdir("/");
if (!@mkdir("testtmpskipifdir")) {
	die("skip for root only");
}
rmdir("testtmpskipifdir");
?>
--FILE--
<?php

var_dump(mkdir("/testdir/subdir", 0777, true));
var_dump(rmdir("/testdir/subdir"));
var_dump(rmdir("/testdir"));

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
bool(true)
Done
