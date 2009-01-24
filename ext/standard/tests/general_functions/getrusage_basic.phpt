--TEST--
Test getrusage() function: basic test
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN" )
  die("skip.. Do not run on Windows");
?>
--FILE--
<?php
/* Prototype  :  array getrusage  ([ int $who  ] )
 * Description: Gets the current resource usages
 * Source code: ext/standard/microtime.c
 * Alias to functions: 
 */

echo "Simple testcase for getrusage() function\n";

$dat = getrusage();

if (!is_array($dat)) {
	echo "TEST FAILED : getrusage shoudl return an array\n";
} 	

// echo the fields which are common to all platforms 
echo "User time used (seconds) " . $dat["ru_utime.tv_sec"] . "\n";
echo "User time used (microseconds) " . $dat["ru_utime.tv_usec"] . "\n"; 
?>
===DONE===
--EXPECTF--
Simple testcase for getrusage() function
User time used (seconds) %d
User time used (microseconds) %d
===DONE===
