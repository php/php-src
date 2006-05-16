--TEST--
recursive mkdir() tests
--FILE--
<?php

var_dump(mkdir("testdir/subdir", 0777, true));
var_dump(rmdir("testdir/subdir"));
var_dump(rmdir("testdir"));

var_dump(mkdir("./testdir/subdir", 0777, true));
var_dump(rmdir("./testdir/subdir"));
var_dump(rmdir("./testdir"));

var_dump(mkdir(dirname(__FILE__)."/testdir/subdir", 0777, true));
var_dump(rmdir(dirname(__FILE__)."/testdir/subdir"));
var_dump(rmdir(dirname(__FILE__)."/testdir"));

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Done
