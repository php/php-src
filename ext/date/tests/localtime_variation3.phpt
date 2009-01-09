--TEST--
Test localtime() function : usage variation - Passing higher positive and negetive float values to timestamp.
--FILE--
<?php
/* Prototype  : array localtime([int timestamp [, bool associative_array]])
 * Description: Returns the results of the C system call localtime as an associative array 
 * if the associative_array argument is set to 1 other wise it is a regular array 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing localtime() : usage variation ***\n";

date_default_timezone_set("UTC");
// Initialise function arguments not being substituted (if any)
$is_associative = true;

echo "\n-- Testing localtime() function with 'float 12.3456789000e10' to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( localtime($timestamp) );
var_dump( localtime($timestamp, $is_associative) );

echo "\n-- Testing localtime() function with 'float -12.3456789000e10' to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( localtime($timestamp) );
var_dump( localtime($timestamp, $is_associative) );

?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing localtime\(\) : usage variation \*\*\*

-- Testing localtime\(\) function with 'float 12.3456789000e10' to timestamp --
array\(9\) {
  \[0\]=>
  int\((16|0)\)
  \[1\]=>
  int\((50|30)\)
  \[2\]=>
  int\((4|0)\)
  \[3\]=>
  int\((26|11)\)
  \[4\]=>
  int\(2\)
  \[5\]=>
  int\((35|3982)\)
  \[6\]=>
  int\((2|6)\)
  \[7\]=>
  int\((84|69)\)
  \[8\]=>
  int\(0\)
}
array\(9\) {
  \["tm_sec"\]=>
  int\((16|0)\)
  \["tm_min"\]=>
  int\((50|30)\)
  \["tm_hour"\]=>
  int\((4|0)\)
  \["tm_mday"\]=>
  int\((26|11)\)
  \["tm_mon"\]=>
  int\(2\)
  \["tm_year"\]=>
  int\((35|3982)\)
  \["tm_wday"\]=>
  int\((2|6)\)
  \["tm_yday"\]=>
  int\((84|69)\)
  \["tm_isdst"\]=>
  int\(0\)
}

-- Testing localtime\(\) function with 'float -12.3456789000e10' to timestamp --
array\(9\) {
  \[0\]=>
  int\((44|52|0)\)
  \[1\]=>
  int\((9|45|30)\)
  \[2\]=>
  int\((19|20|23)\)
  \[3\]=>
  int\((8|13|22)\)
  \[4\]=>
  int\((9|11)\)
  \[5\]=>
  int\((104|1|-3843)\)
  \[6\]=>
  int\((5|-5)\)
  \[7\]=>
  int\((281|346|294)\)
  \[8\]=>
  int\(0\)
}
array\(9\) {
  \["tm_sec"\]=>
  int\((44|52|0)\)
  \["tm_min"\]=>
  int\((9|45|30)\)
  \["tm_hour"\]=>
  int\((19|20|23)\)
  \["tm_mday"\]=>
  int\((8|13|22)\)
  \["tm_mon"\]=>
  int\((9|11)\)
  \["tm_year"\]=>
  int\((104|1|-3843)\)
  \["tm_wday"\]=>
  int\((5|-5)\)
  \["tm_yday"\]=>
  int\((281|346|294)\)
  \["tm_isdst"\]=>
  int\(0\)
}
===DONE===
