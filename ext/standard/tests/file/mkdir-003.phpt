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

var_dump(mkdir(__DIR__."/mkdir-003/subdir", 0777, true));
var_dump(rmdir(__DIR__."/mkdir-003/subdir"));
var_dump(rmdir(__DIR__."/mkdir-003"));

echo "Done\n";
?>
--EXPECT--
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
