--TEST--
mkdir() tests
--FILE--
<?php

var_dump(mkdir("mkdir-001"));
var_dump(mkdir("mkdir-001/subdir"));
var_dump(rmdir("mkdir-001/subdir"));
var_dump(rmdir("mkdir-001"));

var_dump(mkdir("./mkdir-001"));
var_dump(mkdir("./mkdir-001/subdir"));
var_dump(rmdir("./mkdir-001/subdir"));
var_dump(rmdir("./mkdir-001"));

var_dump(mkdir(dirname(__FILE__)."/mkdir-001"));
var_dump(mkdir(dirname(__FILE__)."/mkdir-001/subdir"));
var_dump(rmdir(dirname(__FILE__)."/mkdir-001/subdir"));
var_dump(rmdir(dirname(__FILE__)."/mkdir-001"));

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
