--TEST--
Test popen() and pclose function: error conditions
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != 'Sun')
  die("skip Only valid for Sun Solaris");
?>
--FILE--
<?php

$file_path = __DIR__;
echo "*** Testing for error conditions ***\n";
var_dump( popen() );  // Zero Arguments
var_dump( popen("abc.txt") );   // Single Argument
var_dump( popen("abc.txt", "rw") );   // Invalid mode Argument
var_dump( pclose() );
$file_handle = fopen($file_path."/popen.tmp", "w");
var_dump( pclose($file_handle, $file_handle) );
fclose($file_handle);
var_dump( pclose(1) );
echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/popen.tmp");
?>
--EXPECTF--
*** Testing for error conditions ***

Warning: Wrong parameter count for popen() in %s on line %d
NULL

Warning: Wrong parameter count for popen() in %s on line %d
NULL
sh: abc.txt: not found
resource(%d) of type (stream)

Warning: Wrong parameter count for pclose() in %s on line %d
NULL

Warning: Wrong parameter count for pclose() in %s on line %d
NULL

Warning: pclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

--- Done ---
