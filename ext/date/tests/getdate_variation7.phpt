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
  int\((36|0)\)
  \["minutes"\]=>
  int\((43|0)\)
  \["hours"\]=>
  int\((10|6)\)
  \["mday"\]=>
  int\((26|11)\)
  \["wday"\]=>
  int\((2|6)\)
  \["mon"\]=>
  int\(3\)
  \["year"\]=>
  int\((1935|5882)\)
  \["yday"\]=>
  int\((84|69)\)
  \["weekday"\]=>
  string\((7|8)\) "(Tuesday|Saturday)"
  \["month"\]=>
  string\(5\) "March"
  \[0\]=>
  int\((-1097262584|123456789000)\)
}

-- Testing getdate\(\) function by passing float -12.3456789000e10 value to timestamp --
array\(11\) {
  \["seconds"\]=>
  int\((44|12|20)\)
  \["minutes"\]=>
  int\((39|23)\)
  \["hours"\]=>
  int\((0|2|5)\)
  \["mday"\]=>
  int\((9|14|23)\)
  \["wday"\]=>
  int\((6|-4)\)
  \["mon"\]=>
  int\((10|12)\)
  \["year"\]=>
  int\((2004|1901|-1943)\)
  \["yday"\]=>
  int\((282|347|295)\)
  \["weekday"\]=>
  string\((8|7)\) "(Saturday|Unknown)"
  \["month"\]=>
  string\((7|8)\) "(October|December)"
  \[0\]=>
  int\((1097262584|-2147483648|-123456789000)\)
}
===DONE===
