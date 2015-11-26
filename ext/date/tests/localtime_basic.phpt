--TEST--
Test localtime() function : basic functionality 
--FILE--
<?php
/* Prototype  : array localtime([int timestamp [, bool associative_array]])
 * Description: Returns the results of the C system call localtime as an associative array 
 * if the associative_array argument is set to 1 other wise it is a regular array 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing localtime() : basic functionality ***\n";

date_default_timezone_set("UTC");

// Initialise all required variables
$timestamp = 10;
$associative_array = true;

// Calling localtime() with all possible arguments
var_dump( localtime($timestamp, $associative_array) );

// Calling localtime() with possible optional arguments
var_dump( localtime($timestamp) );

// Calling localtime() with mandatory arguments
var_dump( localtime() );

?>
===DONE===
--EXPECTF--
*** Testing localtime() : basic functionality ***
array(9) {
  ["tm_sec"]=>
  int(10)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_mday"]=>
  int(1)
  ["tm_mon"]=>
  int(0)
  ["tm_year"]=>
  int(70)
  ["tm_wday"]=>
  int(4)
  ["tm_yday"]=>
  int(0)
  ["tm_isdst"]=>
  int(0)
}
array(9) {
  [0]=>
  int(10)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(70)
  [6]=>
  int(4)
  [7]=>
  int(0)
  [8]=>
  int(0)
}
array(9) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
}
===DONE===
