--TEST--
Test popen() and pclose function: error conditions
--SKIPIF--
<?php
if (strtoupper( substr(PHP_OS, 0, 3) ) == 'SUN')
  die("skip Not Valid for Sun Solaris");
?>
--FILE--
<?php
/*
 * Prototype: resource popen ( string command, string mode )
 * Description: Opens process file pointer.

 * Prototype: int pclose ( resource handle );
 * Description: Closes process file pointer.
 */
$file_path = __DIR__;
echo "*** Testing for error conditions ***\n";
var_dump( popen("abc.txt", "rw") );   // Invalid mode Argument
$file_handle = fopen($file_path."/popen.tmp", "w");
fclose($file_handle);
echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/popen.tmp");
?>
--EXPECTF--
*** Testing for error conditions ***

Warning: popen(abc.txt,rw): %s on line %d
bool(false)

--- Done ---
