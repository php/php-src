--TEST--
Test flock() function: Basic functionality
--FILE--
<?php
/*
Prototype: bool flock(resource $handle, int $operation [, int &$wouldblock]);
Description: PHP supports a portable way of locking complete files
  in an advisory way
*/

echo "*** Testing flock() fun with file and dir ***\n";
$file_path = dirname(__FILE__);

$file_handle = fopen("$file_path/lock.tmp", "w");
var_dump(flock($file_handle, LOCK_SH|LOCK_NB));
var_dump(flock($file_handle, LOCK_UN));
var_dump(flock($file_handle, LOCK_EX));
var_dump(flock($file_handle, LOCK_UN));
fclose($file_handle);
unlink("$file_path/lock.tmp");

mkdir("$file_path/dir");
$dir_handle = opendir("$file_path/dir");
var_dump(flock($dir_handle, LOCK_SH|LOCK_NB));
var_dump(flock($dir_handle, LOCK_UN));
var_dump(flock($dir_handle, LOCK_EX));
var_dump(flock($dir_handle, LOCK_UN));
closedir($dir_handle);
rmdir("$file_path/dir");

echo "\n*** Done ***\n";
?>
--EXPECT--
*** Testing flock() fun with file and dir ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)

*** Done ***
