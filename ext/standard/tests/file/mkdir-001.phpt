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

var_dump(mkdir(__DIR__."/mkdir-001"));
var_dump(mkdir(__DIR__."/mkdir-001/subdir"));
var_dump(rmdir(__DIR__."/mkdir-001/subdir"));
var_dump(rmdir(__DIR__."/mkdir-001"));

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
bool(true)
bool(true)
bool(true)
Done
