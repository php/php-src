--TEST--
Test getdate() function : usage variation - Passing high positive and negative float values to timestamp.
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing getdate() : usage variation ***\n";
date_default_timezone_set("Asia/Calcutta");

echo "\n-- Testing getdate() function by passing float 12.3456789000e10 value to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( getdate($timestamp) );

echo "\n-- Testing getdate() function by passing float -12.3456789000e10 value to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( getdate($timestamp) );
?>
===DONE===
--EXPECTREGEX--

\*\*\* Testing getdate\(\) : usage variation \*\*\*

-- Testing getdate\(\) function by passing float 12.3456789000e10 value to timestamp --
array\(11\) {
  \["seconds"\]=>
  int\((7|0)\)
  \["minutes"\]=>
  int\((44|0)\)
  \["hours"\]=>
  int\((8|6)\)
  \["mday"\]=>
  int\((19|11)\)
  \["wday"\]=>
  int\((2|6)\)
  \["mon"\]=>
  int\((1|3)\)
  \["year"\]=>
  int\((2038|5882)\)
  \["yday"\]=>
  int\((18|69)\)
  \["weekday"\]=>
  string\((7|8)\) "(Tuesday|Saturday)"
  \["month"\]=>
  string\((7|5)\) "(January|March)"
  \[0\]=>
  int\((2147483647|123456789000)\)
}

-- Testing getdate\(\) function by passing float -12.3456789000e10 value to timestamp --
array\(11\) {
  \["seconds"\]=>
  int\((12|20)\)
  \["minutes"\]=>
  int\((39|23)\)
  \["hours"\]=>
  int\((2|5)\)
  \["mday"\]=>
  int\((14|23)\)
  \["wday"\]=>
  int\((6|-4)\)
  \["mon"\]=>
  int\((12|10)\)
  \["year"\]=>
  int\((1901|-1943)\)
  \["yday"\]=>
  int\((347|295)\)
  \["weekday"\]=>
  string\((8|7)\) "(Saturday|Unknown)"
  \["month"\]=>
  string\((8|7)\) "(December|October)"
  \[0\]=>
  int\((-2147483648|-123456789000)\)
}
===DONE===
