--TEST--
Test popen() and pclose function: error conditions

--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip Valid only on Windows");
if (PHP_DEBUG) die("skip Not Valid for debug builds");
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
echo "*** Testing for error conditions ***" . PHP_EOL;
var_dump( popen() );  // Zero Arguments
var_dump( popen("abc.txt") );   // Single Argument
var_dump( popen("abc.txt", "rw") );   // Invalid mode Argument
fflush(STDERR);
fflush(STDOUT);
var_dump( pclose() );

fflush(STDERR);
fflush(STDOUT);


$file_handle = fopen($file_path."/popen.tmp", "w");
var_dump( pclose($file_handle, $file_handle) );

fflush(STDERR);
fflush(STDOUT);


pclose($file_handle);
var_dump( pclose(1) );

fflush(STDERR);
fflush(STDOUT);


echo PHP_EOL . PHP_EOL . "--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/popen.tmp");
?>
--EXPECTF--
