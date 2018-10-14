--TEST--
recursive mkdir() tests
--FILE--
<?php

var_dump(mkdir("mkdir-003/subdir", 0777, true));
var_dump(rmdir("mkdir-003/subdir"));
var_dump(rmdir("mkdir-003"));

var_dump(mkdir("./mkdir-003/subdir", 0777, true));
var_dump(rmdir("./mkdir-003/subdir"));
var_dump(rmdir("./mkdir-003"));

var_dump(mkdir(dirname(__FILE__)."/mkdir-003/subdir", 0777, true));
var_dump(rmdir(dirname(__FILE__)."/mkdir-003/subdir"));
var_dump(rmdir(dirname(__FILE__)."/mkdir-003"));

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
