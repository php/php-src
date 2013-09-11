--TEST--
mkdir() tests
--FILE--
<?php

var_dump(mkdir("testdir"));
var_dump(mkdir("testdir/subdir"));
var_dump(rmdir("testdir/subdir"));
var_dump(rmdir("testdir"));

var_dump(mkdir("./testdir"));
var_dump(mkdir("./testdir/subdir"));
var_dump(rmdir("./testdir/subdir"));
var_dump(rmdir("./testdir"));

var_dump(mkdir(dirname(__FILE__)."/testdir"));
var_dump(mkdir(dirname(__FILE__)."/testdir/subdir"));
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
bool(true)
bool(true)
bool(true)
Done
