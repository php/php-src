--TEST--
Test lstat() and stat() functions: error conditions
--FILE--
<?php
echo "*** Testing lstat() for error conditions ***\n";
$file_path = __DIR__;
var_dump( lstat("$file_path/temp.tmp") ); // non existing file
var_dump( lstat(22) ); // scalar argument

echo "\n*** Testing stat() for error conditions ***\n";

var_dump( stat("$file_path/temp.tmp") ); // non existing file
var_dump( stat("$file_path/temp/") ); // non existing dir
var_dump( stat(22) ); // scalar argument

echo "Done\n";
?>
--EXPECTF--
*** Testing lstat() for error conditions ***

Warning: lstat(): Lstat failed for %s in %s on line %d
bool(false)

Warning: lstat(): Lstat failed for 22 in %s on line %d
bool(false)

*** Testing stat() for error conditions ***

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for 22 in %s on line %d
bool(false)
Done
