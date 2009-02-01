--TEST--
Test getmypid() function: basic test
--FILE--
<?php
/* Prototype  : int getmypid  ( void  )
 * Description: Gets the current PHP process ID.
 * Source code: ext/standard/pageinfo.c
 * Alias to functions: 
 */

echo "Simple testcase for getmypid() function\n";

var_dump(getmypid());

// getmypid has not checks for spurious args 
var_dump(getmypid("foo"));

echo "Done\n";
?>
--EXPECTF--
Simple testcase for getmypid() function
int(%d)
int(%d)
Done