--TEST--
Test popen() and pclose function: error conditions
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' && PHP_DEBUG) die("skip Valid only on Windows");
if(!PHP_DEBUG) die("skip Not Valid for release builds");

ob_start();phpinfo(INFO_GENERAL);$inf=ob_get_contents(); ob_end_clean();
if (!(strpos($inf, 'MSVC9') || strpos($inf, 'MSVC8'))) die("skip Not Valid for build done with VC < 8");
?>
--FILE--
<?php
/*
 * Prototype: resource popen ( string command, string mode )
 * Description: Opens process file pointer.

 * Prototype: int pclose ( resource handle );
 * Description: Closes process file pointer.
 */
$file_path = dirname(__FILE__);
echo "*** Testing for error conditions ***\n";
var_dump( popen() );  // Zero Arguments
var_dump( popen("abc.txt") );   // Single Argument
var_dump( popen("abc.txt", "rw") );   // Invalid mode Argument
var_dump( pclose() );
$file_handle = fopen($file_path."/popen.tmp", "w");
var_dump( pclose($file_handle, $file_handle) );
pclose($file_handle);
var_dump( pclose(1) );
echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/popen.tmp");
?>
--EXPECTF--
*** Testing for error conditions ***

Warning: popen() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: popen() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: Invalid parameter detected in CRT function '_fdopen' (%s:%d) in %s on line %d

Warning: popen(abc.txt,rw): Invalid argument in %s on line %d
bool(false)

Warning: pclose() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: pclose() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: pclose() expects parameter 1 to be resource, integer given in %s on line %d
bool(false)

--- Done ---'abc.txt' is not recognized as an internal or external command,
operable program or batch file.
