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
  \[u"seconds"\]=>
  int\((36|0)\)
  \[u"minutes"\]=>
  int\((43|0)\)
  \[u"hours"\]=>
  int\((10|6)\)
  \[u"mday"\]=>
  int\((26|11)\)
  \[u"wday"\]=>
  int\((2|6)\)
  \[u"mon"\]=>
  int\(3\)
  \[u"year"\]=>
  int\((1935|5882)\)
  \[u"yday"\]=>
  int\((84|69)\)
  \[u"weekday"\]=>
  unicode\((7|8)\) "(Tuesday|Saturday)"
  \[u"month"\]=>
  unicode\(5\) "March"
  \[0\]=>
  int\((-1097262584|123456789000)\)
}

-- Testing getdate\(\) function by passing float -12.3456789000e10 value to timestamp --
array\(11\) {
  \[u"seconds"\]=>
  int\((44|12|20)\)
  \[u"minutes"\]=>
  int\((39|23)\)
  \[u"hours"\]=>
  int\((0|2|5)\)
  \[u"mday"\]=>
  int\((9|14|23)\)
  \[u"wday"\]=>
  int\((6|-4)\)
  \[u"mon"\]=>
  int\((10|12)\)
  \[u"year"\]=>
  int\((2004|1901|-1943)\)
  \[u"yday"\]=>
  int\((282|347|295)\)
  \[u"weekday"\]=>
  unicode\((8|7)\) "(Saturday|Unknown)"
  \[u"month"\]=>
  unicode\((7|8)\) "(October|December)"
  \[0\]=>
  int\((1097262584|-2147483648|-123456789000)\)
}
===DONE===
