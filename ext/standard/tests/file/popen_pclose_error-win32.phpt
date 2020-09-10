--TEST--
Test popen() and pclose function: error conditions on Windows
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN')
  die("skip only valid for Windows");
?>
--FILE--
<?php
$file_path = __DIR__;
echo "*** Testing for error conditions ***\n";

popen("abc.txt", "rw");   // Invalid mode Argument

$file_handle = fopen($file_path."/popen.tmp", "w");
fclose($file_handle);
echo "\n--- Done ---";

?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/popen.tmp");
?>
--EXPECT--
*** Testing for error conditions ***

Warning: popen(abc.txt,rw): No such file or directory in %s on line %d

--- Done ---
