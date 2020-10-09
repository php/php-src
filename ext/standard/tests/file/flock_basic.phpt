--TEST--
Test flock() function: Basic functionality
--FILE--
<?php
/*
Description: PHP supports a portable way of locking complete files
  in an advisory way
*/

echo "*** Testing flock() fun with file and dir ***\n";

$lock_file = preg_replace("~\.phpt?$~", null, __FILE__);

$file_handle = fopen($lock_file, "w");
var_dump(flock($file_handle, LOCK_SH|LOCK_NB));
var_dump(flock($file_handle, LOCK_UN));
var_dump(flock($file_handle, LOCK_EX));
var_dump(flock($file_handle, LOCK_UN));
fclose($file_handle);
unlink($lock_file);

$lock_dir = sprintf("%s.dir", preg_replace("~\.phpt?$~", null, __FILE__));

mkdir($lock_dir);
$dir_handle = opendir($lock_dir);
var_dump(flock($dir_handle, LOCK_SH|LOCK_NB));
var_dump(flock($dir_handle, LOCK_UN));
var_dump(flock($dir_handle, LOCK_EX));
var_dump(flock($dir_handle, LOCK_UN));
closedir($dir_handle);
rmdir($lock_dir);

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
