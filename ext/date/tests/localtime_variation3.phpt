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
  int\((7|0)\)
  \[1\]=>
  int\((14|30)\)
  \[2\]=>
  int\((3|0)\)
  \[3\]=>
  int\((19|11)\)
  \[4\]=>
  int\((0|2)\)
  \[5\]=>
  int\((138|3982)\)
  \[6\]=>
  int\((2|6)\)
  \[7\]=>
  int\((18|69)\)
  \[8\]=>
  int\(0\)
}
array\(9\) {
  \[u"tm_sec"\]=>
  int\((7|0)\)
  \[u"tm_min"\]=>
  int\((14|30)\)
  \[u"tm_hour"\]=>
  int\((3|0)\)
  \[u"tm_mday"\]=>
  int\((19|11)\)
  \[u"tm_mon"\]=>
  int\((0|2)\)
  \[u"tm_year"\]=>
  int\((138|3982)\)
  \[u"tm_wday"\]=>
  int\((2|6)\)
  \[u"tm_yday"\]=>
  int\((18|69)\)
  \[u"tm_isdst"\]=>
  int\(0\)
}

-- Testing localtime\(\) function with 'float -12.3456789000e10' to timestamp --
array\(9\) {
  \[0\]=>
  int\((52|0)\)
  \[1\]=>
  int\((45|30)\)
  \[2\]=>
  int\((20|23)\)
  \[3\]=>
  int\((13|22)\)
  \[4\]=>
  int\((11|9)\)
  \[5\]=>
  int\((1|-3843)\)
  \[6\]=>
  int\((5|-5)\)
  \[7\]=>
  int\((346|294)\)
  \[8\]=>
  int\(0\)
}
array\(9\) {
  \[u"tm_sec"\]=>
  int\((52|0)\)
  \[u"tm_min"\]=>
  int\((45|30)\)
  \[u"tm_hour"\]=>
  int\((20|23)\)
  \[u"tm_mday"\]=>
  int\((13|22)\)
  \[u"tm_mon"\]=>
  int\((11|9)\)
  \[u"tm_year"\]=>
  int\((1|-3843)\)
  \[u"tm_wday"\]=>
  int\((5|-5)\)
  \[u"tm_yday"\]=>
  int\((346|294)\)
  \[u"tm_isdst"\]=>
  int\(0\)
}
===DONE===
