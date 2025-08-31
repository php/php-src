--TEST--
Test stat() function: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
$file_path = __DIR__;
$arr = array(__FILE__);

echo "\n*** Testing stat() for error conditions ***\n";

var_dump( stat("$file_path/temp.tmp") ); // non existing file
var_dump( stat("$file_path/temp/") ); // non existing dir
var_dump( stat(22) ); // scalar argument

echo "Done\n";
?>
--EXPECTF--
*** Testing stat() for error conditions ***

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for %s in %s on line %d
bool(false)

Warning: stat(): stat failed for 22 in %s on line %d
bool(false)
Done
