--TEST--
Test getdate() function : basic functionality 
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information 
 * Source code: ext/date/php_date.c
 */

echo "*** Testing getdate() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("Asia/Calcutta");

// Initialise all required variables
$timestamp = 10;

// Calling getdate() with all possible arguments
var_dump( getdate($timestamp) );

// Calling getdate() with mandatory arguments
var_dump( getdate() );

?>
===DONE===
--EXPECTF--
*** Testing getdate() : basic functionality ***
array(11) {
  ["seconds"]=>
  int(10)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(10)
}
array(11) {
  ["seconds"]=>
  int(%d)
  ["minutes"]=>
  int(%d)
  ["hours"]=>
  int(%d)
  ["mday"]=>
  int(%d)
  ["wday"]=>
  int(%d)
  ["mon"]=>
  int(%d)
  ["year"]=>
  int(%d)
  ["yday"]=>
  int(%d)
  ["weekday"]=>
  string(%d) %s
  ["month"]=>
  string(%d) %s
  [0]=>
  int(%d)
}
===DONE===
